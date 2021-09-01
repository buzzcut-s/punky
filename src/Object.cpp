#include "../include/Object.hpp"

#include <any>
#include <string>
#include <variant>

namespace punky::obj
{

std::string inspect(const Object& obj)
{
    switch (obj.m_type)
    {
        case ObjectType::Int:
            return std::to_string(std::get<int>(obj.m_value));

        case ObjectType::Boolean:
            return std::get<bool>(obj.m_value) ? "true" : "false";

        case ObjectType::Return:
            return inspect(std::any_cast<Object>(obj));

        case ObjectType::Error:
            return std::get<std::string>(obj.m_value);

        case ObjectType::Null:
            return "null";
    }
}

std::string type_to_string(const ObjectType& type)
{
    switch (type)
    {
        case ObjectType::Int:
            return "int";

        case ObjectType::Boolean:
            return "boolean";

        case ObjectType::Return:
            return "return";

        case ObjectType::Error:
            return "error";

        case ObjectType::Null:
            return "null";
    }
}

}  // namespace punky::obj
