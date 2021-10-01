#ifndef LEXER_HPP
#define LEXER_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "Token.hpp"

namespace punky::lex
{

using punky::tok::Token;

class Lexer
{
public:
    explicit Lexer(std::string line);

    Token next_token();

private:
    std::string                 m_line;
    std::string::const_iterator m_curr_it;
    char                        m_curr_char;

    [[nodiscard]] bool next_eof() const;

    [[nodiscard]] auto peek() const -> std::optional<char>;

    void consume();

    void skip_whitespace();

    std::string tokenize_identifier();
    std::string tokenize_integer();
};
}  // namespace punky::lex

#endif  // LEXER_HPP
