
#include "database.hpp"

void Database::set(std::string key, std::string value)
{
    store_.insert_or_assign(
        std::move(key),
        std::move(value)
    );
}

bool Database::get(std::string_view key, std::string& value) const
{
    const auto it = store_.find(key);

    if (it == store_.end())
    {
        return false;
    }

    value = it->second;
    return true;
}

bool Database::exists(std::string_view key) const
{
    return store_.contains(key);
}

bool Database::erase(std::string_view key)
{
    auto it = store_.find(key);

    if (it == store_.end())
    {
        return false;
    }
    
    store_.erase(it);
    return true;
}

void Database::clear()
{
    store_.clear();
}
