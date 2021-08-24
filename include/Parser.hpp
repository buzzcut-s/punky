#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Token.hpp"

namespace ast
{
class Program;
class StmtNode;
class LetStmt;
}  // namespace ast

namespace parser
{

class Parser
{
public:
    explicit Parser(Lexer lex);

    void consume();
    auto errors() const { return m_errors; }

    auto parse_program() -> std::unique_ptr<ast::Program>;

private:
    Lexer m_lex;
    Token m_curr_tok{};
    Token m_peek_tok{};

    std::vector<std::string> m_errors{};

    auto parse_statement() -> std::unique_ptr<ast::StmtNode>;
    auto parse_let_statement() -> std::unique_ptr<ast::LetStmt>;

    bool curr_type_is(const TokenType& type) const;
    bool peek_type_is(const TokenType& type) const;

    bool expect_peek(const TokenType& type);
    void peek_error(const TokenType& type);
};

}  // namespace parser

#endif  // PARSER_HPP
