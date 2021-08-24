#include "../include/Parser.hpp"

#include <charconv>
#include <memory>
#include <utility>

#include "../include/Lexer.hpp"
#include "../include/ast.hpp"

namespace parser
{

Parser::Parser(Lexer lex) :
  m_lex{std::move(lex)}
{
    consume();
    consume();

    register_prefix(TokenType::Identifier, [this] { return parse_identifier(); });
    register_prefix(TokenType::Int, [this] { return parse_int_literal(); });
}

void Parser::consume()
{
    m_curr_tok = m_peek_tok;
    m_peek_tok = m_lex.next_token();
}

auto Parser::parse_program() -> std::unique_ptr<ast::Program>
{
    auto prog = std::make_unique<ast::Program>();
    while (!curr_type_is(TokenType::EOS))
    {
        if (auto stmt = parse_statement(); stmt)
            prog->push_stmt(std::move(stmt));
        consume();
    }
    return prog;
}

auto Parser::parse_statement() -> std::unique_ptr<ast::StmtNode>
{
    switch (m_curr_tok.m_type)
    {
        case TokenType::Let:
            return parse_let_statement();
        case TokenType::Return:
            return parse_return_statement();
        default:
            return parse_expression_statement();
    }
}

auto Parser::parse_let_statement() -> std::unique_ptr<ast::LetStmt>
{
    auto let_stmt = std::make_unique<ast::LetStmt>(m_curr_tok);

    if (!expect_peek(TokenType::Identifier))
        return nullptr;

    auto ident = ast::Identifier(std::move(m_curr_tok));
    let_stmt->set_ident(std::move(ident));

    if (!expect_peek(TokenType::Equal))
        return nullptr;
    consume();

    while (!curr_type_is(TokenType::Semicolon) && !curr_type_is(TokenType::EOS))
        consume();

    return let_stmt;
}

auto Parser::parse_return_statement() -> std::unique_ptr<ast::ReturnStmt>
{
    auto ret_stmt = std::make_unique<ast::ReturnStmt>(m_curr_tok);

    consume();

    while (!curr_type_is(TokenType::Semicolon) && !curr_type_is(TokenType::EOS))
        consume();

    return ret_stmt;
}

auto Parser::parse_expression_statement() -> std::unique_ptr<ast::ExpressionStmt>
{
    auto expression_stmt = std::make_unique<ast::ExpressionStmt>(m_curr_tok);

    auto expression = parse_expression(PrecedenceLevel::Lowest);
    expression_stmt->set_expression(std::move(expression));

    if (peek_type_is(TokenType::Semicolon))
        consume();

    return expression_stmt;
}

auto Parser::parse_expression(PrecedenceLevel prec_lv) -> std::unique_ptr<ast::ExprNode>
{
    auto prefix_fn = m_prefix_parse_fns[m_curr_tok.m_type];

    if (!prefix_fn)
        return nullptr;

    auto left_exp = prefix_fn();
    return left_exp;
}

auto Parser::parse_identifier() -> std::unique_ptr<ast::ExprNode>
{
    return std::make_unique<ast::Identifier>(std::move(m_curr_tok));
}

auto Parser::parse_int_literal() -> std::unique_ptr<ast::ExprNode>
{
    std::string_view token_val{};
    if (m_curr_tok.m_literal.has_value())
        token_val = m_curr_tok.m_literal.value();

    int int_val{};
    if (const auto [p, ec] =
          std::from_chars(token_val.data(), token_val.data() + token_val.size(), int_val);
        ec == std::errc())
    {
        return std::make_unique<ast::IntLiteral>(std::move(m_curr_tok), int_val);
    }

    m_errors.emplace_back("Could not parse "
                          + std::string{token_val}
                          + " as integer\n");
    return nullptr;
}

bool Parser::curr_type_is(const TokenType& type) const
{
    return m_curr_tok.m_type == type;
}

bool Parser::peek_type_is(const TokenType& type) const
{
    return m_peek_tok.m_type == type;
}

void Parser::peek_error(const TokenType& type)
{
    m_errors.emplace_back("Expected next token to be ["
                          + type_to_string(type)
                          + "], but got ["
                          + type_to_string(m_peek_tok.m_type)
                          + "] instead\n");
}

void Parser::register_prefix(TokenType type, PrefixParseFn parse_fn)
{
    m_prefix_parse_fns[type] = std::move(parse_fn);
}

void Parser::register_infix(TokenType type, InfixParseFn parse_fn)
{
    m_infix_parse_fns[type] = std::move(parse_fn);
}

bool Parser::expect_peek(const TokenType& type)
{
    if (peek_type_is(type))
    {
        consume();
        return true;
    }
    peek_error(type);
    return false;
}

}  // namespace parser
