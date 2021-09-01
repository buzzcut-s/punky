#include "../include/Environment.hpp"

#include <optional>
#include <string>
#include <utility>

namespace punky::env
{

obj::Object Environment::set(std::string name, const obj::Object& value)
{
    m_store[std::move(name)] = value;
    return value;
}

auto Environment::get(const std::string& name) const -> std::optional<obj::Object>
{
    if (const auto res = m_store.find(name); res != m_store.cend())
        return res->second;
    return std::nullopt;
}

}  // namespace punky::env
