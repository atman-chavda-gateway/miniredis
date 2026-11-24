#include "client_session.hpp"

#include <memory>
#include <utility>

int main()
{
    try
    {
        asio::io_context io_context;
        
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 8080);
        asio::ip::tcp::acceptor acceptor(io_context, endpoint);

        std::println("miniredis is listening on port {}", endpoint.port());

        const auto accept_connection = [&](auto&& self)-> void
        {
            acceptor.async_accept([&](const std::error_code& error, asio::ip::tcp::socket socket)
                {
                    if(!error)
                    {
                        auto session = std::make_shared<ClientSession>(std::move(socket));
                        session->start();
                    }
                    else
                    {
                        std::println("Accept error: {}", error.message());
                    }
                    self(self);
                }
            );            
        };
        accept_connection(accept_connection);
        io_context.run();
    }
    catch(const std::exception& e)
    {
        std::println("Exception: {}", e.what());
        return 1;
    }
    return 0;
}