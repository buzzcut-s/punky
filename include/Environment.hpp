#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "Object.hpp"

namespace punky::env
{

class Environment
{
public:
    Environment() = default;
    explicit Environment(Environment* outer) :
      m_outer{outer}
    {}

    auto set(std::string name, const obj::Object& value) -> obj::Object;
    auto get(const std::string& name) const -> std::optional<obj::Object>;

private:
    std::unordered_map<std::string, obj::Object> m_store;

    Environment* m_outer{};
};
}  // namespace punky::env

#endif  // ENVIRONMENT_HPP
