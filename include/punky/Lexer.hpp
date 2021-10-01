#ifndef LEXER_HPP
#define LEXER_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "Token.hpp"

namespace punky::lex
{

using punky::tok::Token;
using punky::tok::TokenType;

class Lexer
{
public:
    explicit Lexer(std::string line);

    Token next_token();

private:
    std::string                 m_line;
    std::string::const_iterator m_curr_it;
    char                        m_curr_char;

    std::unordered_map<std::string, TokenType> m_keywords;

    [[nodiscard]] bool next_eof() const;

    [[nodiscard]] auto peek() const -> std::optional<char>;

    void consume();

    void skip_whitespace();

    std::string tokenize_identifier();
    std::string tokenize_integer();

    [[nodiscard]] auto token_type(const std::string& tok) const -> TokenType;
};
}  // namespace punky::lex

#endif  // LEXER_HPP
