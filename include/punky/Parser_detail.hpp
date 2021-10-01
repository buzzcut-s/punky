#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

#include "Lexer.hpp"
#include "Token.hpp"
#include "ast.hpp"

namespace punky::par
{

enum class PrecedenceLevel
{
    _,  // 0
    Lowest,
    Equals,       // ==
    LessGreater,  // > or <
    Sum,          // + or -
    Product,      // * or /
    Prefix,       // -x or !x
    Call,         // myFunc(x)
    Index,        // index
};

enum class ParseFnType
{
    Prefix,
    Infix
};

}  // namespace punky::par

#endif  // PARSER_COMMON_HPP
