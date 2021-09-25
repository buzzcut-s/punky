#ifndef PARSER_HPP
#define PARSER_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Lexer.hpp"
#include "Token.hpp"

namespace punky::ast
{
class ExprNode;
class Program;
class StmtNode;
class LetStmt;
class ReturnStmt;
class ExpressionStmt;
class BlockStmt;
class Identifier;
using ExprNodePtr    = std::unique_ptr<ExprNode>;
using ExprNodeVector = std::vector<ExprNodePtr>;
using OptFnParams    = std::optional<std::unique_ptr<std::vector<Identifier>>>;
using OptCallArgs    = std::optional<std::unique_ptr<ExprNodeVector>>;
using OptIfAltBlk    = std::optional<std::unique_ptr<BlockStmt>>;
}  // namespace punky::ast

namespace punky::par
{

using punky::lex::Lexer;
using punky::tok::Token;
using punky::tok::TokenType;

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
    explicit Parser(lex::Lexer lex);

    auto errors() const { return m_errors; }

    auto parse_program() -> std::unique_ptr<ast::Program>;

private:
    using PrefixParseFn = std::function<ast::ExprNodePtr()>;
    using InfixParseFn  = std::function<ast::ExprNodePtr(ast::ExprNodePtr)>;

    Lexer m_lex;
    Token m_curr_tok;
    Token m_peek_tok;

    std::vector<std::string> m_errors;

    std::unordered_map<TokenType, PrefixParseFn> m_prefix_parse_fns;
    std::unordered_map<TokenType, InfixParseFn>  m_infix_parse_fns;

    std::unordered_map<TokenType, PrecedenceLevel> m_precedences;

    void consume();

    auto parse_statement() -> std::unique_ptr<ast::StmtNode>;
    auto parse_let_statement() -> std::unique_ptr<ast::LetStmt>;
    auto parse_return_statement() -> std::unique_ptr<ast::ReturnStmt>;
    auto parse_expression_statement() -> std::unique_ptr<ast::ExpressionStmt>;
    auto parse_block_statement() -> std::unique_ptr<ast::BlockStmt>;

    auto parse_expression(PrecedenceLevel precedence) -> ast::ExprNodePtr;

    auto parse_prefix_expression() -> ast::ExprNodePtr;
    auto parse_infix_expression(ast::ExprNodePtr left_expr) -> ast::ExprNodePtr;

    auto parse_identifier() -> ast::ExprNodePtr;
    auto parse_int_literal() -> ast::ExprNodePtr;
    auto parse_boolean() -> ast::ExprNodePtr;
    auto parse_grouped_expression() -> ast::ExprNodePtr;
    auto parse_if_expression() -> ast::ExprNodePtr;

    auto parse_function_literal() -> ast::ExprNodePtr;
    auto parse_function_params() -> ast::OptFnParams;

    auto parse_call_expression(ast::ExprNodePtr function) -> ast::ExprNodePtr;
    auto parse_call_arguments() -> ast::OptCallArgs;

    [[nodiscard]] bool curr_type_is(const TokenType& type) const;
    [[nodiscard]] bool peek_type_is(const TokenType& type) const;

    bool expect_peek(const TokenType& type);
    void peek_error(const TokenType& type);

    void register_prefix(TokenType type, PrefixParseFn pre_parse_fn);
    void register_infix(TokenType type, InfixParseFn in_parse_fn);

    [[nodiscard]] auto curr_precedence() const -> PrecedenceLevel;
    [[nodiscard]] auto peek_precedence() const -> PrecedenceLevel;
};
}  // namespace punky::par

#endif  // PARSER_HPP