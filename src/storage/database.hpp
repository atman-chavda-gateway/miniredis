#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

// Enables zero-allocation lookups using std::string_view
struct StringHash
{
    using is_transparent = void;

    [[nodiscard]] size_t operator()(std::string_view sv) const
    {
        return std::hash<std::string_view>{}(sv);
    }
};

class Database
{
public:
    void set(std::string key, std::string value);

    [[nodiscard]]
    bool get(std::string_view key, std::string& value) const;

    [[nodiscard]]
    bool exists(std::string_view key) const;

    [[nodiscard]]
    bool erase(std::string_view key);

    void clear();
    
private:
    // Pass StringHash and std::equal_to<> to unlock string_view support
    std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> store_;
};
