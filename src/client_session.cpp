
#include "client_session.hpp"

#include <print>
#include <system_error>
#include <utility>

ClientSession::ClientSession(asio::ip::tcp::socket socket)
    : socket_(std::move(socket))
{
}

void ClientSession::start()
{
    std::error_code error;
    const auto endpoint = socket_.remote_endpoint(error);

    if (!error)
    {
        std::println(
            "Client connected: {}:{}",
            endpoint.address().to_string(),
            endpoint.port()
        );
    }

    do_read();
}

void ClientSession::do_read()
{
    auto self = shared_from_this();

    socket_.async_read_some(
        asio::buffer(buffer_),
        [self](const std::error_code& error, std::size_t bytes_transferred)
        {
            if (bytes_transferred > 0)
            {
                self->input_buffer_.append(
                    self->buffer_.data(),
                    bytes_transferred
                );

                self->process_input();
            }

            if (error)
            {
                if (error == asio::error::eof)
                {
                    std::println("Client disconnected.");
                }
                else if (error != asio::error::operation_aborted)
                {
                    std::println("Read error: {}", error.message());
                }

                return;
            }

            self->do_read();
        }
    );
}

void ClientSession::process_input()
{
    while (!input_buffer_.empty())
    {
        const auto result = parser_.parse(input_buffer_);

        if (result.status == ParseStatus::incomplete)
        {
            // Wait for more TCP data.
            return;
        }

        if (result.status == ParseStatus::invalid)
        {
            std::println("Invalid RESP request. Closing connection.");

            std::error_code error;
            socket_.close(error);
            return;
        }

        if (result.bytes_consumed == 0 ||
            result.bytes_consumed > input_buffer_.size())
        {
            std::println("Parser returned invalid consumed byte count.");

            std::error_code error;
            socket_.close(error);
            return;
        }

        std::println("Received command:");

        for (std::size_t i = 0; i < result.arguments.size(); ++i)
        {
            std::println("  [{}]: {}", i, result.arguments[i]);
        }

        // Remove only the bytes belonging to this command.
        input_buffer_.erase(0, result.bytes_consumed);
    }
}
