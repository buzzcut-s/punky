#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <variant>

namespace punky::obj
{

enum class ObjectType
{
    Null,
    Int,
    Boolean,
};

struct Object
{
    ObjectType                              m_type{};
    std::variant<std::monostate, int, bool> m_value;
};

std::string inspect(const Object& obj);

}  // namespace punky::obj

#endif  // OBJECT_HPP
