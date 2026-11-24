#include "client_session.hpp"

#include <print>
#include <string_view>
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

    if(!error)
    {
        std::println("Client connected: {}:{}", endpoint.address().to_string(), endpoint.port());        
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

            if(bytes_transferred > 0)
            {
                std::string_view recived(
                    self->buffer_.data(),
                    bytes_transferred
                );

                std::println("Received {} bytes: {}", bytes_transferred, recived);
            }

            if(error)
            {
                if(error == asio::error::eof)
                {
                    std::println("client disconnected");
                }
                else if (error != asio::error::operation_aborted)
                {
                    std::println("Read error: {}", error.message());
                }
                return;

            }

        }
    );
}