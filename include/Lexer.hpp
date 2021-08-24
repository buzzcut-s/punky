#ifndef LEXER_HPP
#define LEXER_HPP

#include <optional>
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
    std::string                 m_line;
    std::string::const_iterator m_it;
    char                        m_ch;

    std::unordered_map<std::string, TokenType> m_keywords;

    void consume();
    auto peek() const -> std::optional<char>;

    void skip_whitespace();

    std::string read_identifier();
    std::string read_number();

    auto token_type(const std::string& tok) const -> TokenType;
};

#endif  // LEXER_HPP
