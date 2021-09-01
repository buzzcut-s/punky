#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <any>
#include <string>
#include <variant>

namespace punky::obj
{

enum class ObjectType
{
    Null,
    Int,
    Boolean,
    Return,
};

using ValVariant = std::variant<std::monostate, int, bool, std::any>;

struct Object
{
    ObjectType m_type;
    ValVariant m_value;
};

std::string inspect(const Object& obj);

}  // namespace punky::obj

#endif  // OBJECT_HPP
