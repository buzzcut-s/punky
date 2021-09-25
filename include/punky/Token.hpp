#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <optional>
#include <string>
#include <unordered_map>

namespace punky::tok
{

// clang-format off
enum class TokenType
{
    // Single-character tokens
    LeftParen, RightParen, LeftBracket,
    RightBracket, LeftBrace, RightBrace,

    Comma, Dot, Minus, Plus,
    Slash, Asterisk, Colon, Semicolon,

    // One or two character tokens
    Bang, BangEqual,
    Equal, EqualEqual,
    Greater, GreaterEqual,
    Less, LessEqual,

    // Literals
    Identifier,
    String, Int,

    // Keywords
    Func, Let,
    True, False,
    If, Else,
    Return,

    // Utils
    Illegal,
    EOS,
};
// clang-format on

struct Token
{
    TokenType                  m_type{};
    std::optional<std::string> m_literal;
};

auto make_token(TokenType type, std::optional<std::string> literal) -> Token;

auto make_keywords() -> std::unordered_map<std::string, TokenType>;

std::string type_to_string(const TokenType& type);

}  // namespace punky::tok

#endif  // TOKEN_HPP