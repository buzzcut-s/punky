#include "../include/Evaluator.hpp"

#include <iostream>
#include <memory>
#include <utility>

#include "../include/Object.hpp"
#include "../include/Token.hpp"
#include "../include/ast.hpp"

namespace punky::eval
{

using punky::tok::TokenType;

using obj::Object;
using obj::ObjectType;

using punky::ast::AstType;

Evaluator::Evaluator(std::unique_ptr<ast::Program> prog) :
  m_root{std::move(prog)}
{
}

Object Evaluator::interpret()
{
    Object result{};
    for (const auto& stmt : m_root->m_statements)
        result = eval(stmt.get());
    return result;
}

void Evaluator::print() const
{
    if (m_root)
        std::cout << m_root->to_string();
}

Object Evaluator::eval(ast::AstNode* node)
{
    switch (node->ast_type())
    {
        case AstType::ExpressionStmt:
            std::cout << "ExprStmt\n";
            return eval(node->expr_stmt()->expression());

        case AstType::Int:
            std::cout << "IntLit\n";
            return Object{ObjectType::Int, node->int_lit()->value()};

        case AstType::Bool:
            std::cout << "Bool\n";
            return Object{ObjectType::Boolean, node->boolean()->value()};

        case AstType::Prefix:
        {
            const auto right = eval(node->prefix_expr()->right());
            return eval_prefix_expr(node->expr()->type(), right);
        }

        case AstType::Infix:
        {
            const auto left  = eval(node->infix_expr()->left());
            const auto right = eval(node->infix_expr()->right());
            return eval_infix_expr(node->expr()->type(), left, right);
        }

        default:
            std::cout << "DefExpr\n";
            return Object{ObjectType::Null, std::monostate{}};
    }
}

obj::Object Evaluator::eval_prefix_expr(const tok::TokenType& op, const Object& right)
{
    switch (op)
    {
        case TokenType::Bang:
            return eval_bang_prefix_expr(right);

        case TokenType::Minus:
            return eval_minus_prefix_expr(right);

        default:
            return Object{ObjectType::Null, std::monostate{}};
    }
}

obj::Object Evaluator::eval_bang_prefix_expr(const Object& right)
{
    switch (right.m_type)
    {
        case ObjectType::Boolean:
            return Object{ObjectType::Boolean, !std::get<bool>(right.m_value)};

        case ObjectType::Null:
            return Object{ObjectType::Boolean, true};

        default:
            return Object{ObjectType::Boolean, false};
    }
}

obj::Object Evaluator::eval_minus_prefix_expr(const obj::Object& right)
{
    if (right.m_type == ObjectType::Int)
        return Object{ObjectType::Int, -std::get<int>(right.m_value)};

    return Object{ObjectType::Null, std::monostate{}};
}

obj::Object Evaluator::eval_infix_expr(const tok::TokenType& op,
                                       const obj::Object& left, const obj::Object& right)
{
    if (left.m_type == ObjectType::Int && right.m_type == ObjectType::Int)
        return eval_int_infix_expr(op, left, right);

    if (left.m_type == ObjectType::Boolean && right.m_type == ObjectType::Boolean)
        return eval_bool_infix_expr(op, left, right);

    return Object{ObjectType::Null, std::monostate{}};
}

obj::Object Evaluator::eval_int_infix_expr(const tok::TokenType& op,
                                           const obj::Object& left, const obj::Object& right)
{
    const auto left_val  = std::get<int>(left.m_value);
    const auto right_val = std::get<int>(right.m_value);

    switch (op)
    {
        case TokenType::Plus:
            return Object{ObjectType::Int, left_val + right_val};

        case TokenType::Minus:
            return Object{ObjectType::Int, left_val - right_val};

        case TokenType::Asterisk:
            return Object{ObjectType::Int, left_val * right_val};

        case TokenType::Slash:
            return Object{ObjectType::Int, left_val / right_val};

        case TokenType::Less:
            return Object{ObjectType::Boolean, left_val < right_val};

        case TokenType::Greater:
            return Object{ObjectType::Boolean, left_val > right_val};

        case TokenType::EqualEqual:
            return Object{ObjectType::Boolean, left_val == right_val};

        case TokenType::BangEqual:
            return Object{ObjectType::Boolean, left_val != right_val};

        default:
            return Object{ObjectType::Null, std::monostate{}};
    }
}

obj::Object Evaluator::eval_bool_infix_expr(const tok::TokenType& op,
                                            const obj::Object& left, const obj::Object& right)
{
    const auto left_val  = std::get<bool>(left.m_value);
    const auto right_val = std::get<bool>(right.m_value);

    switch (op)
    {
        case TokenType::EqualEqual:
            return Object{ObjectType::Boolean, left_val == right_val};

        case TokenType::BangEqual:
            return Object{ObjectType::Boolean, left_val != right_val};

        default:
            return Object{ObjectType::Null, std::monostate{}};
    }
}

}  // namespace punky::eval
