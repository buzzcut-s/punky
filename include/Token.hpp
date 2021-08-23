#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <optional>
#include <string>
#include <unordered_map>

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
    String, Number, Int,

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

Token make_token(TokenType type, std::optional<std::string> literal);

std::unordered_map<std::string, TokenType> make_keywords();

std::string token_to_string(const Token& tok);

#endif  // TOKEN_HPP
