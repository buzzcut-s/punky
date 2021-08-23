#include "../include/Lexer.hpp"

#include <string>

#include "../include/Token.hpp"
#include "../include/utils.hpp"

Lexer::Lexer(std::string line) :
  m_keywords{make_keywords()},
  m_line{std::move(line)},
  m_read_iter{m_line.begin()}
{
}

Token Lexer::next_token()
{
    skip_whitespace();

    auto tok = Token{};
    switch (*m_read_iter)
    {
        case '=':
            if (peek() == '=')
            {
                consume();
                tok = make_token(TokenType::EqualEqual, std::nullopt);
            }
            else
                tok = make_token(TokenType::Equal, std::nullopt);
            break;
        case '!':
            if (peek() == '=')
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
                auto ident = read_identifier();
                auto type  = token_type(ident);
                return make_token(type, std::move(ident));
            }
            else if (utils::is_digit(*m_read_iter))
                return make_token(TokenType::Number, read_number());
            else
                tok = make_token(TokenType::Illegal, std::nullopt);
            break;
    }

    consume();
    return tok;
}

void Lexer::consume()
{
    if (m_read_iter == m_line.end())
        return;

    ++m_read_iter;
}

char Lexer::peek()
{
    if (m_read_iter + 1 == m_line.end())
        return 0;
    return *(m_read_iter + 1);
}

void Lexer::skip_whitespace()
{
    while (utils::is_whitespace(*m_read_iter))
        consume();
}

std::string Lexer::read_identifier()
{
    std::string ident;
    while (utils::is_letter(*m_read_iter))
    {
        ident.push_back(*m_read_iter);
        consume();
    }
    return ident;
}

std::string Lexer::read_number()
{
    std::string num;
    while (utils::is_digit(*m_read_iter))
    {
        num.push_back(*m_read_iter);
        consume();
    }
    return num;
}

TokenType Lexer::token_type(const std::string& tok)
{
    if (auto res = m_keywords.find(tok); res != m_keywords.cend())
        return res->second;
    return TokenType::Identifier;
}
