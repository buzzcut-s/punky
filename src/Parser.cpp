#include "punky/Parser.hpp"

#include <charconv>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include <punky/Lexer.hpp>
#include <punky/Token.hpp>
#include <punky/ast.hpp>

namespace punky::par
{

static constexpr auto precedence_lookup(TokenType type) -> PrecedenceLevel;

Parser::Parser(Lexer lex) :
  m_lex{std::move(lex)}
{
    consume();
    consume();

    register_prefix(TokenType::Identifier, [this] { return parse_identifier(); });
    register_prefix(TokenType::Int, [this] { return parse_int_literal(); });
    register_prefix(TokenType::Bang, [this] { return parse_prefix_expression(); });
    register_prefix(TokenType::Minus, [this] { return parse_prefix_expression(); });
    register_prefix(TokenType::True, [this] { return parse_boolean(); });
    register_prefix(TokenType::False, [this] { return parse_boolean(); });
    register_prefix(TokenType::LeftParen, [this] { return parse_grouped_expression(); });
    register_prefix(TokenType::If, [this] { return parse_if_expression(); });
    register_prefix(TokenType::Func, [this] { return parse_function_literal(); });

    register_infix(TokenType::Plus, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Minus, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Asterisk, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Slash, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::EqualEqual, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::BangEqual, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Less, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::Greater, [this](ast::ExprNodePtr left_expr) {
        return parse_infix_expression(std::move(left_expr));
    });
    register_infix(TokenType::LeftParen, [this](ast::ExprNodePtr function) {
        return parse_call_expression(std::move(function));
    });
}

void Parser::consume()
{
    std::swap(m_curr_tok, m_peek_tok);
    m_peek_tok = m_lex.next_token();
}

auto Parser::parse_program() -> std::variant<bool, std::unique_ptr<ast::Program>>
{
    auto prog = std::make_unique<ast::Program>();
    while (!curr_type_is(TokenType::EOS))
    {
        if (auto stmt = parse_statement(); stmt)
            prog->push_stmt(std::move(stmt));
        consume();
    }

    if (!m_errors.empty())
    {
        std::cerr << "parser errors:\n";
        for (const auto& error : m_errors)
            std::cerr << "\t" + error + "\n";
        return false;  // TODO(piyush): Throw instead
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

    if (!expect_peek_and_consume(TokenType::Identifier))
        return nullptr;

    auto ident = ast::Identifier(std::move(m_curr_tok));

    if (!expect_peek_and_consume(TokenType::Equal))
        return nullptr;
    consume();

    auto value = parse_expression(PrecedenceLevel::Lowest);

    if (peek_type_is(TokenType::Semicolon))
        consume();

    return std::make_unique<ast::LetStmt>(let_tok, std::move(ident), std::move(value));
}

auto Parser::parse_return_statement() -> std::unique_ptr<ast::ReturnStmt>
{
    auto ret_tok = m_curr_tok;
    consume();
    auto ret_value = parse_expression(PrecedenceLevel::Lowest);

    if (peek_type_is(TokenType::Semicolon))
        consume();

    return std::make_unique<ast::ReturnStmt>(ret_tok, std::move(ret_value));
}

auto Parser::parse_expression_statement() -> std::unique_ptr<ast::ExpressionStmt>
{
    auto expr_tok   = m_curr_tok;  // Not moving, parse_expr() needs m_curr_tok
    auto expression = parse_expression(PrecedenceLevel::Lowest);

    if (peek_type_is(TokenType::Semicolon))
        consume();

    return std::make_unique<ast::ExpressionStmt>(std::move(expr_tok), std::move(expression));
}

auto Parser::parse_block_statement() -> std::unique_ptr<ast::BlockStmt>
{
    auto blk_tok = m_curr_tok;
    auto blk     = std::make_unique<ast::BlockStmt>(blk_tok);
    consume();
    while (!curr_type_is(TokenType::RightBrace))
    {
        if (curr_type_is(TokenType::EOS))
        {
            m_errors.emplace_back("Block statement missing closing '}'");
            return nullptr;
        }
        auto stmt = parse_statement();
        if (stmt)
            blk->push_stmt(std::move(stmt));
        consume();
    }
    return blk;
}

auto Parser::parse_expression(PrecedenceLevel precedence) -> ast::ExprNodePtr
{
    auto prefix_fn = m_prefix_parse_fns[m_curr_tok.m_type];
    if (!prefix_fn)
        return parse_fn_error(m_curr_tok.m_type, ParseFnType::Prefix);

    auto left_expr = prefix_fn();
    while (!peek_type_is(TokenType::Semicolon) && precedence < peek_precedence())
    {
        auto infix_fn = m_infix_parse_fns[m_peek_tok.m_type];
        if (!infix_fn)
        {
            parse_fn_error(m_curr_tok.m_type, ParseFnType::Infix);
            return left_expr;
        }
        consume();
        left_expr = infix_fn(std::move(left_expr));
    }

    return left_expr;
}

auto Parser::parse_prefix_expression() -> ast::ExprNodePtr
{
    auto prefix_tok = m_curr_tok;
    consume();
    auto right_expr = parse_expression(PrecedenceLevel::Prefix);
    return std::make_unique<ast::PrefixExpression>(prefix_tok, std::move(right_expr));
}

auto Parser::parse_infix_expression(ast::ExprNodePtr left_expr) -> ast::ExprNodePtr
{
    auto infix_tok  = m_curr_tok;
    auto precedence = curr_precedence();
    consume();
    auto right_expr = parse_expression(precedence);
    return std::make_unique<ast::InfixExpression>(infix_tok,
                                                  std::move(left_expr), std::move(right_expr));
}

auto Parser::parse_identifier() -> ast::ExprNodePtr
{
    return std::make_unique<ast::Identifier>(std::move(m_curr_tok));
}

auto Parser::parse_int_literal() -> ast::ExprNodePtr
{
    std::string_view buff{m_curr_tok.m_literal.value()};

    int int_val{};
    if (const auto [p, ec] =
          std::from_chars(buff.data(), buff.data() + buff.size(), int_val);
        ec == std::errc())
    {
        return std::make_unique<ast::IntLiteral>(std::move(m_curr_tok), int_val);
    }

    m_errors.emplace_back("Could not parse " + std::string{buff} + " as integer");
    return nullptr;
}

auto Parser::parse_boolean() -> ast::ExprNodePtr
{
    auto bool_tok = m_curr_tok;
    bool bool_val = m_curr_tok.m_type == TokenType::True;
    return std::make_unique<ast::Boolean>(bool_tok, bool_val);
}

auto Parser::parse_grouped_expression() -> ast::ExprNodePtr
{
    consume();
    auto expression = parse_expression(PrecedenceLevel::Lowest);

    if (!expect_peek_and_consume(TokenType::RightParen))
        return nullptr;

    return expression;
}

auto Parser::parse_if_expression() -> ast::ExprNodePtr
{
    auto if_tok = m_curr_tok;

    if (!expect_peek_and_consume(TokenType::LeftParen))
        return nullptr;
    consume();

    auto condition = parse_expression(PrecedenceLevel::Lowest);

    if (!expect_peek_and_consume(TokenType::RightParen))
        return nullptr;
    if (!expect_peek_and_consume(TokenType::LeftBrace))
        return nullptr;

    auto consequence = parse_block_statement();

    ast::OptIfAltBlk alternative;
    if (peek_type_is(TokenType::Else))
    {
        consume();
        if (!expect_peek_and_consume(TokenType::LeftBrace))
            return nullptr;

        alternative = parse_block_statement();
    }

    return std::make_unique<ast::IfExpression>(if_tok, std::move(condition),
                                               std::move(consequence), std::move(alternative));
}

auto Parser::parse_function_literal() -> ast::ExprNodePtr
{
    auto func_tok = m_curr_tok;

    if (!expect_peek_and_consume(TokenType::LeftParen))
        return nullptr;

    auto params = parse_function_params();

    if (!expect_peek_and_consume(TokenType::LeftBrace))
        return nullptr;

    auto body = parse_block_statement();

    return std::make_unique<ast::FunctionLiteral>(func_tok, std::move(params), std::move(body));
}

auto Parser::parse_function_params() -> ast::OptFnParams
{
    if (peek_type_is(TokenType::RightParen))
    {
        consume();
        return std::nullopt;
    }
    consume();

    auto ident = ast::Identifier(std::move(m_curr_tok));

    std::vector<ast::Identifier> params{std::move(ident)};
    while (peek_type_is(TokenType::Comma))
    {
        consume();
        consume();
        ident = ast::Identifier(std::move(m_curr_tok));
        params.push_back(std::move(ident));
    }

    if (!expect_peek_and_consume(TokenType::RightParen))
        return nullptr;

    return std::make_unique<std::vector<ast::Identifier>>(std::move(params));
}

auto Parser::parse_call_expression(ast::ExprNodePtr function) -> ast::ExprNodePtr
{
    auto call_tok  = m_curr_tok;
    auto arguments = parse_call_arguments();
    return std::make_unique<ast::CallExpression>(call_tok,
                                                 std::move(function), std::move(arguments));
}

auto Parser::parse_call_arguments() -> ast::OptCallArgs
{
    if (peek_type_is(TokenType::RightParen))
    {
        consume();
        return std::nullopt;
    }
    consume();

    auto args_expr = parse_expression(PrecedenceLevel::Lowest);

    ast::ExprNodeVector args;
    args.push_back(std::move(args_expr));
    while (peek_type_is(TokenType::Comma))
    {
        consume();
        consume();
        args_expr = parse_expression(PrecedenceLevel::Lowest);
        args.push_back(std::move(args_expr));
    }

    if (!expect_peek_and_consume(TokenType::RightParen))
        return nullptr;

    return std::make_unique<ast::ExprNodeVector>(std::move(args));
}

bool Parser::curr_type_is(const TokenType& type) const
{
    return m_curr_tok.m_type == type;
}

bool Parser::peek_type_is(const TokenType& type) const
{
    return m_peek_tok.m_type == type;
}

bool Parser::expect_peek_and_consume(const TokenType& type)
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
    m_errors.emplace_back("Expected next token to be "
                          + tok::type_to_string(type)
                          + ", but got "
                          + tok::type_to_string(m_peek_tok.m_type)
                          + " instead");
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
    return precedence_lookup(m_curr_tok.m_type);
}

auto Parser::peek_precedence() const -> PrecedenceLevel
{
    return precedence_lookup(m_peek_tok.m_type);
}

ast::ExprNodePtr Parser::parse_fn_error(TokenType tok_type, ParseFnType parse_type)
{
    if (parse_type == ParseFnType::Infix)
        m_errors.emplace_back("No infix parse function found for token '"
                              + tok::type_to_string(tok_type) + "'");
    else
        m_errors.emplace_back("No prefix parse function found for token '"
                              + tok::type_to_string(tok_type) + "'");
    return nullptr;
}

static constexpr auto precedence_lookup(TokenType type) -> PrecedenceLevel
{
    switch (type)
    {
        case TokenType::EqualEqual:
        case TokenType::BangEqual:
            return PrecedenceLevel::Equals;
        case TokenType::Less:
        case TokenType::Greater:
            return PrecedenceLevel::LessGreater;
        case TokenType::Plus:
        case TokenType::Minus:
            return PrecedenceLevel::Sum;
        case TokenType::Asterisk:
        case TokenType::Slash:
            return PrecedenceLevel::Product;
        case TokenType::LeftParen:
            return PrecedenceLevel::Call;
        case TokenType::LeftBracket:
            return PrecedenceLevel::Index;
        default:
            return PrecedenceLevel::Lowest;
    }
}

}  // namespace punky::par
