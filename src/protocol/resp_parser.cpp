
#include "resp_parser.hpp"

#include <charconv>
#include <system_error>

namespace
{
    constexpr std::size_t max_array_elements = 1024;
    constexpr std::size_t max_bulk_string_size = 1024 * 1024; // 1 MiB

    enum class ReadStatus
    {
        complete,
        incomplete,
        invalid
    };

    struct IntegerResult
    {
        ReadStatus status;
        std::size_t value = 0;
        std::size_t bytes_consumed = 0;
    };

    IntegerResult read_integer(std::string_view input)
    {
        const auto terminator = input.find("\r\n");

        if (terminator == std::string_view::npos)
        {
            return {ReadStatus::incomplete};
        }

        if (terminator == 0)
        {
            return {ReadStatus::invalid};
        }

        std::size_t value = 0;

        const auto* begin = input.data();
        const auto* end = begin + terminator;

        const auto [ptr, error] = std::from_chars(begin, end, value);

        if (error != std::errc{} || ptr != end)
        {
            return {ReadStatus::invalid};
        }

        return {
            ReadStatus::complete,
            value,
            terminator + 2
        };
    }

    struct BulkStringResult
    {
        ReadStatus status;
        std::string value;
        std::size_t bytes_consumed = 0;
    };

    BulkStringResult read_bulk_string(std::string_view input)
    {
        if (input.empty())
        {
            return {ReadStatus::incomplete};
        }

        if (input.front() != '$')
        {
            return {ReadStatus::invalid};
        }

        input.remove_prefix(1);

        const auto length = read_integer(input);

        if (length.status != ReadStatus::complete)
        {
            return {length.status};
        }

        if (length.value > max_bulk_string_size)
        {
            return {ReadStatus::invalid};
        }

        const auto header_size = length.bytes_consumed;

        // Header + payload + trailing CRLF.
        if (input.size() - header_size < length.value + 2)
        {
            return {ReadStatus::incomplete};
        }

        const auto payload = input.substr(header_size, length.value);

        const auto terminator = input.substr(
            header_size + length.value,
            2
        );

        if (terminator != "\r\n")
        {
            return {ReadStatus::invalid};
        }

        return {
            ReadStatus::complete,
            std::string(payload),
            1 + header_size + length.value + 2
        };
    }
}

ParseResult RespParser::parse(std::string_view input) const
{
    if (input.empty())
    {
        return {ParseStatus::incomplete};
    }

    if (input.front() != '*')
    {
        return {ParseStatus::invalid};
    }

    input.remove_prefix(1);

    const auto array_length = read_integer(input);

    if (array_length.status == ReadStatus::incomplete)
    {
        return {ParseStatus::incomplete};
    }

    if (array_length.status == ReadStatus::invalid)
    {
        return {ParseStatus::invalid};
    }

    if (array_length.value == 0 ||
        array_length.value > max_array_elements)
    {
        return {ParseStatus::invalid};
    }

    std::size_t position = 1 + array_length.bytes_consumed;

    std::vector<std::string> arguments;
    arguments.reserve(array_length.value);
    
    for (std::size_t i = 0; i < array_length.value; ++i)
    {
        const auto remaining = input.substr(position - 1);
    
        const auto bulk_string = read_bulk_string(remaining);
    
        if (bulk_string.status == ReadStatus::incomplete)
        {
            return {ParseStatus::incomplete};
        }
    
        if (bulk_string.status == ReadStatus::invalid)
        {
            return {ParseStatus::invalid};
        }
    
        arguments.push_back(bulk_string.value);
        position += bulk_string.bytes_consumed;
    }
    
    return {
        ParseStatus::complete,
        std::move(arguments),
        position
    };
}
