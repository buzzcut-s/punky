#include "../include/Token.hpp"

#include <string>
#include <unordered_map>

auto make_token(TokenType type, std::optional<std::string> literal) -> Token
{
    return Token{type, std::move(literal)};
}

auto make_keywords() -> std::unordered_map<std::string, TokenType>
{
    return std::unordered_map<std::string, TokenType>{
      {"fn", TokenType::Func},
      {"let", TokenType::Let},
      {"true", TokenType::True},
      {"false", TokenType::False},
      {"if", TokenType::If},
      {"else", TokenType::Else},
      {"return", TokenType::Return},
    };
}

std::string type_to_string(const TokenType& type)
{
    switch (type)
    {
        case TokenType::LeftParen:
            return "LEFT_PAREN";
        case TokenType::RightParen:
            return "RIGHT_PAREN";
        case TokenType::LeftBracket:
            return "LEFT_BRACKET";
        case TokenType::RightBracket:
            return "RIGHT_BRACKET";
        case TokenType::LeftBrace:
            return "LEFT_BRACE";
        case TokenType::RightBrace:
            return "RIGHT_BRACE";
        case TokenType::Comma:
            return "COMMA";
        case TokenType::Dot:
            return "DOT";
        case TokenType::Minus:
            return "-";
        case TokenType::Plus:
            return "+";
        case TokenType::Slash:
            return "/";
        case TokenType::Asterisk:
            return "*";
        case TokenType::Colon:
            return ":";
        case TokenType::Semicolon:
            return ";";
        case TokenType::Bang:
            return "!";
        case TokenType::BangEqual:
            return "!=";
        case TokenType::Equal:
            return "=";
        case TokenType::EqualEqual:
            return "==";
        case TokenType::Greater:
            return ">";
        case TokenType::GreaterEqual:
            return ">=";
        case TokenType::Less:
            return "<";
        case TokenType::LessEqual:
            return "<=";
        case TokenType::Identifier:
            return "IDENTIFIER - UNREACHABLE";
        case TokenType::String:
            return "STRING";
        case TokenType::Int:
            return "INT - UNREACHABLE";
        case TokenType::Func:
            return "FUNC";
        case TokenType::Let:
            return "LET";
        case TokenType::True:
            return "true";
        case TokenType::False:
            return "false";
        case TokenType::If:
            return "IF";
        case TokenType::Else:
            return "ELSE";
        case TokenType::Return:
            return "RETURN";
        case TokenType::Illegal:
            return "ILLEGAL - UNREACHABLE";
        case TokenType::EOS:
            return "EOS - UNREACHABLE";
        default:
            return "DEFAULT - UNREACHABLE";
    }
}

std::string format_token(const Token& tok)
{
    std::string tok_str{"{"};
    tok_str.append(" type: " + type_to_string(tok.m_type) + ",");
    if (tok.m_literal.has_value())
        tok_str.append(" literal: " + tok.m_literal.value() + " ");
    tok_str.append("}");
    return tok_str;
}
