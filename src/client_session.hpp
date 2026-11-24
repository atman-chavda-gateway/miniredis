#pragma once

#include <asio.hpp>
#include <memory>
#include <array>

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
    public:
        explicit ClientSession(asio::ip::tcp::socket socket);

        void start();

    private:
        void do_read();

        asio::ip::tcp::socket socket_;
        std::array<char, 4096> buffer_{};
};