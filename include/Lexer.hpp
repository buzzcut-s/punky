#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <unordered_map>

class Token;
enum class TokenType;

class Lexer
{
public:
    explicit Lexer(std::string line);

    Token next_token();

private:
    std::unordered_map<std::string, TokenType> m_keywords;

    std::string           m_line;
    std::string::iterator m_read_iter;

    void consume();
    char peek();
    void skip_whitespace();

    std::string read_identifier();
    std::string read_number();

    TokenType token_type(const std::string& tok);
};

#endif  // LEXER_HPP
