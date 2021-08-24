#include "../include/Lexer.hpp"

#include <iterator>
#include <optional>
#include <string>

#include "../include/Token.hpp"
#include "../include/utils.hpp"

Lexer::Lexer(std::string line) :
  m_line{std::move(line)},
  m_read_iter{m_line.begin()},
  m_keywords{make_keywords()}
{
}

Token Lexer::next_token()
{
    skip_whitespace();

    auto tok = Token{};
    switch (*m_read_iter)
    {
        case '=':
            if (peek().has_value() && peek().value() == '=')
            {
                consume();
                tok = make_token(TokenType::EqualEqual, std::nullopt);
            }
            else
                tok = make_token(TokenType::Equal, std::nullopt);
            break;
        case '!':
            if (peek().has_value() && peek().value() == '=')
            {
                consume();
                tok = make_token(TokenType::BangEqual, std::nullopt);
            }
            else
                tok = make_token(TokenType::Bang, std::nullopt);
            break;
        case '+':
            tok = make_token(TokenType::Plus, std::nullopt);
            break;
        case '-':
            tok = make_token(TokenType::Minus, std::nullopt);
            break;
        case '/':
            tok = make_token(TokenType::Slash, std::nullopt);
            break;
        case '*':
            tok = make_token(TokenType::Asterisk, std::nullopt);
            break;
        case '<':
            tok = make_token(TokenType::Less, std::nullopt);
            break;
        case '>':
            tok = make_token(TokenType::Greater, std::nullopt);
            break;
        case ';':
            tok = make_token(TokenType::Semicolon, std::nullopt);
            break;
        case '(':
            tok = make_token(TokenType::LeftParen, std::nullopt);
            break;
        case ')':
            tok = make_token(TokenType::RightParen, std::nullopt);
            break;
        case ',':
            tok = make_token(TokenType::Comma, std::nullopt);
            break;
        case '{':
            tok = make_token(TokenType::LeftBrace, std::nullopt);
            break;
        case '}':
            tok = make_token(TokenType::RightBrace, std::nullopt);
            break;
        case 0:
            tok = make_token(TokenType::EOS, std::nullopt);
            break;
        default:
            if (utils::is_letter(*m_read_iter))
            {
                auto       ident = read_identifier();
                const auto type  = token_type(ident);
                return type == TokenType::Identifier ? make_token(type, std::move(ident))
                                                     : make_token(type, std::nullopt);
            }
            else if (utils::is_digit(*m_read_iter))
                return make_token(TokenType::Int, read_number());
            else
                tok = make_token(TokenType::Illegal, std::nullopt);
            break;
    }

    consume();
    return tok;
}

void Lexer::consume()
{
    if (m_read_iter != m_line.end())
        ++m_read_iter;
}

auto Lexer::peek() const -> std::optional<char>
{
    if (std::next(m_read_iter) != m_line.end())
        return *std::next(m_read_iter);
    return std::nullopt;
}

void Lexer::skip_whitespace()
{
    while (utils::is_whitespace(*m_read_iter))
        consume();
}

std::string Lexer::read_identifier()
{
    const auto ident_begin = m_read_iter;
    while (utils::is_letter(*m_read_iter))
        consume();
    return std::string{ident_begin, m_read_iter};
}

std::string Lexer::read_number()
{
    const auto num_begin = m_read_iter;
    while (utils::is_digit(*m_read_iter))
        consume();
    return std::string{num_begin, m_read_iter};
}

auto Lexer::token_type(const std::string& tok) const -> TokenType
{
    if (const auto res = m_keywords.find(tok); res != m_keywords.end())
        return res->second;
    return TokenType::Identifier;
}
