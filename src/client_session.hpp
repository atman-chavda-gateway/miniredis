
#pragma once

#include <asio.hpp>

#include "protocol/resp_parser.hpp"

#include <array>
#include <memory>
#include <string>

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
    explicit ClientSession(asio::ip::tcp::socket socket);

    void start();

private:
    void do_read();
    void process_input();

    asio::ip::tcp::socket socket_;
    std::array<char, 4096> buffer_{};

    std::string input_buffer_;
    RespParser parser_;
};
