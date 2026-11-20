#include<asio.hpp>

#include<exception>
#include<print>

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
                        const auto remote = socket.remote_endpoint();
                        std::println("Accepted connection from {}:{}", remote.address().to_string(), remote.port());
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