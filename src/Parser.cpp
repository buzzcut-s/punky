#include "../include/Parser.hpp"

#include <charconv>
#include <functional>
#include <memory>
#include <utility>

#include "../include/Lexer.hpp"
#include "../include/ast.hpp"

namespace parser
{

Parser::Parser(Lexer lex) :
  m_lex{std::move(lex)},
  m_precedences{{
    {TokenType::EqualEqual, PrecedenceLevel::Equals},
    {TokenType::BangEqual, PrecedenceLevel::Equals},
    {TokenType::Less, PrecedenceLevel::LessGreater},
    {TokenType::Greater, PrecedenceLevel::LessGreater},
    {TokenType::Plus, PrecedenceLevel::Sum},
    {TokenType::Minus, PrecedenceLevel::Sum},
    {TokenType::Asterisk, PrecedenceLevel::Product},
    {TokenType::Slash, PrecedenceLevel::Product},
    {TokenType::LeftParen, PrecedenceLevel::Call},
    {TokenType::LeftBracket, PrecedenceLevel::Index},
  }}
{
    consume();
    consume();

    register_prefix(TokenType::Identifier, [this] { return parse_identifier(); });
    register_prefix(TokenType::Int, [this] { return parse_int_literal(); });
    register_prefix(TokenType::Bang, [this] { return parse_prefix_expression(); });
    register_prefix(TokenType::Minus, [this] { return parse_prefix_expression(); });

    register_infix(TokenType::Plus, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Minus, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Asterisk, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Slash, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::EqualEqual, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::BangEqual, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Less, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Greater, [this](ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
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
    auto let_tok = m_curr_tok;

    if (!expect_peek(TokenType::Identifier))
        return nullptr;

    auto ident    = ast::Identifier(std::move(m_curr_tok));
    auto let_stmt = std::make_unique<ast::LetStmt>(let_tok, std::move(ident));

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
    auto expression_tok = m_curr_tok;

    auto expression      = parse_expression(PrecedenceLevel::Lowest);
    auto expression_stmt = std::make_unique<ast::ExpressionStmt>(std::move(expression_tok),
                                                                 std::move(expression));

    if (peek_type_is(TokenType::Semicolon))
        consume();

    return expression_stmt;
}

auto Parser::parse_expression(PrecedenceLevel precedence) -> ExprNodePtr
{
    auto prefix_fn = m_prefix_parse_fns[m_curr_tok.m_type];

    if (!prefix_fn)
    {
        m_errors.emplace_back("No prefix parse function found for token '"
                              + type_to_string(m_curr_tok.m_type)
                              + "'\n");
        return nullptr;
    }

    auto left_expr = prefix_fn();
    while (!peek_type_is(TokenType::Semicolon) && precedence < peek_precedence())
    {
        auto infix_fn = m_infix_parse_fns[m_peek_tok.m_type];
        if (!infix_fn)
        {
            m_errors.emplace_back("No infix parse function found for token '"
                                  + type_to_string(m_peek_tok.m_type)
                                  + "'\n");
            return nullptr;
        }
        consume();
        left_expr = infix_fn(std::move(left_expr));
    }

    return left_expr;
}

auto Parser::parse_identifier() -> ExprNodePtr
{
    return std::make_unique<ast::Identifier>(std::move(m_curr_tok));
}

auto Parser::parse_int_literal() -> ExprNodePtr
{
    std::string_view int_tok{};
    if (m_curr_tok.m_literal.has_value())
        int_tok = m_curr_tok.m_literal.value();

    int int_val{};
    if (const auto [p, ec] =
          std::from_chars(int_tok.data(), int_tok.data() + int_tok.size(), int_val);
        ec == std::errc())
    {
        return std::make_unique<ast::IntLiteral>(std::move(m_curr_tok), int_val);
    }

    m_errors.emplace_back("Could not parse "
                          + std::string{int_tok}
                          + " as integer\n");
    return nullptr;
}

auto Parser::parse_prefix_expression() -> ExprNodePtr
{
    auto prefix_tok = m_curr_tok;
    consume();
    auto right_expr = parse_expression(PrecedenceLevel::Prefix);
    return std::make_unique<ast::PrefixExpression>(prefix_tok, std::move(right_expr));
}

auto Parser::parse_infix_expression(ExprNodePtr left_expr) -> ExprNodePtr
{
    auto infix_tok  = m_curr_tok;
    auto precedence = curr_precedence();
    consume();
    auto right_expr = parse_expression(precedence);
    return std::make_unique<ast::InfixExpression>(infix_tok,
                                                  std::move(left_expr), std::move(right_expr));
}

bool Parser::curr_type_is(const TokenType& type) const
{
    return m_curr_tok.m_type == type;
}

bool Parser::peek_type_is(const TokenType& type) const
{
    return m_peek_tok.m_type == type;
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

void Parser::peek_error(const TokenType& type)
{
    m_errors.emplace_back("Expected next token to be ["
                          + type_to_string(type)
                          + "], but got ["
                          + type_to_string(m_peek_tok.m_type)
                          + "] instead\n");
}

void Parser::register_prefix(TokenType type, PrefixParseFn pre_parse_fn)
{
    m_prefix_parse_fns[type] = std::move(pre_parse_fn);
}

void Parser::register_infix(TokenType type, InfixParseFn in_parse_fn)
{
    m_infix_parse_fns[type] = std::move(in_parse_fn);
}

auto Parser::curr_precedence() const -> PrecedenceLevel
{
    if (const auto curr_prec = m_precedences.find(m_curr_tok.m_type);
        curr_prec != m_precedences.cend())
        return curr_prec->second;
    return PrecedenceLevel::Lowest;
}

auto Parser::peek_precedence() const -> PrecedenceLevel
{
    if (const auto peek_prec = m_precedences.find(m_peek_tok.m_type);
        peek_prec != m_precedences.cend())
        return peek_prec->second;
    return PrecedenceLevel::Lowest;
}

}  // namespace parser
