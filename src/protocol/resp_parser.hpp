
#pragma once

#include <string>
#include <string_view>
#include <vector>

enum class ParseStatus
{
    complete,
    incomplete,
    invalid
};

struct ParseResult
{
    ParseStatus status;
    std::vector<std::string> arguments;
    std::size_t bytes_consumed = 0;
};

class RespParser
{
public:
    [[nodiscard]]
    ParseResult parse(std::string_view input) const;
};
