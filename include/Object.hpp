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
    Error,
    EmptyOut
};

using ValVariant = std::variant<std::monostate, int, bool, std::any, std::string>;

struct Object
{
    ObjectType m_type;
    ValVariant m_value;
};

std::string inspect(const Object& obj);

std::string type_to_string(const ObjectType& type);

}  // namespace punky::obj

#endif  // OBJECT_HPP
