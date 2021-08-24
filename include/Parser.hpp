#ifndef PARSER_HPP
#define PARSER_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Lexer.hpp"
#include "Token.hpp"
#include "ast.hpp"

namespace ast
{
class Program;
class StmtNode;
class LetStmt;
class ReturnStmt;
}  // namespace ast

namespace parser
{

enum class PrecedenceLevel
{
    _,  // 0
    Lowest,
    Equals,       // ==
    LessGreater,  // > or <
    Sum,          // + or -
    Product,      // * or /
    Prefix,       // -x or !x
    Call,         // myFunc(x)
    Index,        // index
};

class Parser
{
public:
    explicit Parser(Lexer lex);

    void consume();
    auto errors() const { return m_errors; }

    auto parse_program() -> std::unique_ptr<ast::Program>;

private:
    using ExprNodePtr   = std::unique_ptr<ast::ExprNode>;
    using PrefixParseFn = std::function<ExprNodePtr()>;
    using InfixParseFn  = std::function<ExprNodePtr(ExprNodePtr)>;

    Lexer m_lex;
    Token m_curr_tok{};
    Token m_peek_tok{};

    std::vector<std::string> m_errors{};

    std::unordered_map<TokenType, PrefixParseFn> m_prefix_parse_fns;
    std::unordered_map<TokenType, InfixParseFn>  m_infix_parse_fns;

    auto parse_statement() -> std::unique_ptr<ast::StmtNode>;
    auto parse_let_statement() -> std::unique_ptr<ast::LetStmt>;
    auto parse_return_statement() -> std::unique_ptr<ast::ReturnStmt>;
    auto parse_expression_statement() -> std::unique_ptr<ast::ExpressionStmt>;
    auto parse_expression(PrecedenceLevel prec_lv) -> std::unique_ptr<ast::ExprNode>;

    auto parse_identifier() -> std::unique_ptr<ast::ExprNode>;
    auto parse_int_literal() -> std::unique_ptr<ast::ExprNode>;

    bool curr_type_is(const TokenType& type) const;
    bool peek_type_is(const TokenType& type) const;

    bool expect_peek(const TokenType& type);
    void peek_error(const TokenType& type);

    void register_prefix(TokenType type, PrefixParseFn parse_fn);
    void register_infix(TokenType type, InfixParseFn parse_fn);
};

}  // namespace parser

#endif  // PARSER_HPP
