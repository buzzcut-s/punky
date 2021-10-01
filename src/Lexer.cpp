#include "punky/Lexer.hpp"

#include <iterator>
#include <optional>
#include <string>

#include <punky/Token.hpp>
#include <punky/utils.hpp>

namespace punky::lex
{

Lexer::Lexer(std::string line) :
  m_line{std::move(line)},
  m_curr_it{m_line.cbegin()},
  m_curr_char{*m_curr_it},
  m_keywords{tok::make_keywords()}
{
}

Token Lexer::next_token()
{
    skip_whitespace();

    auto tok = Token{};
    switch (m_curr_char)
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
        // clang-format off
        case '+': tok = make_token(TokenType::Plus, std::nullopt); break;
        case '-': tok = make_token(TokenType::Minus, std::nullopt); break;
        case '/': tok = make_token(TokenType::Slash, std::nullopt); break;
        case '*': tok = make_token(TokenType::Asterisk, std::nullopt); break;
        case '<': tok = make_token(TokenType::Less, std::nullopt); break;
        case '>': tok = make_token(TokenType::Greater, std::nullopt); break;
        case ';': tok = make_token(TokenType::Semicolon, std::nullopt); break;
        case '(': tok = make_token(TokenType::LeftParen, std::nullopt); break;
        case ')': tok = make_token(TokenType::RightParen, std::nullopt); break;
        case ',': tok = make_token(TokenType::Comma, std::nullopt); break;
        case '{': tok = make_token(TokenType::LeftBrace, std::nullopt); break;
        case '}': tok = make_token(TokenType::RightBrace, std::nullopt); break;
        case  0 : return make_token(TokenType::EOS, std::nullopt);
        // clang-format on
        default:
            if (utils::is_letter(m_curr_char))
            {
                auto       ident = tokenize_identifier();
                const auto type  = token_type(ident);
                return type == TokenType::Identifier ? make_token(type, std::move(ident))
                                                     : make_token(type, std::nullopt);
            }
            else if (utils::is_digit(m_curr_char))
                return make_token(TokenType::Int, tokenize_integer());
            else
                tok = make_token(TokenType::Illegal, std::nullopt);
            break;
    }

    consume();
    return tok;
}

bool Lexer::next_eof() const
{
    return std::next(m_curr_it) == m_line.cend();
}

void Lexer::consume()
{
    if (!next_eof())
        m_curr_char = *(++m_curr_it);
    else
        m_curr_char = 0;
}

auto Lexer::peek() const -> std::optional<char>
{
    if (!next_eof())
        return *std::next(m_curr_it);
    return std::nullopt;
}

void Lexer::skip_whitespace()
{
    while (utils::is_whitespace(m_curr_char))
        consume();
}

std::string Lexer::tokenize_identifier()
{
    const auto ident_begin = m_curr_it;
    while (utils::is_letter(m_curr_char))
        consume();
    return std::string{ident_begin, m_curr_it};
}

std::string Lexer::tokenize_integer()
{
    const auto num_begin = m_curr_it;
    while (utils::is_digit(m_curr_char))
        consume();
    return std::string{num_begin, m_curr_it};
}

auto Lexer::token_type(const std::string& tok) const -> TokenType
{
    if (const auto res = m_keywords.find(tok); res != m_keywords.cend())
        return res->second;
    return TokenType::Identifier;
}

}  // namespace punky::lex
