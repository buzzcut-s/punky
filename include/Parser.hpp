#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "Lexer.hpp"
#include "Token.hpp"

namespace ast
{
class Program;
}

namespace parser
{

class Parser
{
    explicit Parser(Lexer lex);

    void next_token();

    auto parse_program() -> std::unique_ptr<ast::Program>;

private:
    Lexer m_lex;
    Token m_curr_tok{};
    Token m_peek_tok{};
};

}  // namespace parser

#endif  // PARSER_HPP
