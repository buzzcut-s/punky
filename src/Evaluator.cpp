#include "../include/Evaluator.hpp"

#include <memory>
#include <utility>

#include "../include/Object.hpp"
#include "../include/Token.hpp"
#include "../include/ast.hpp"

namespace punky::eval
{

using punky::ast::AstType;
using punky::obj::Object;
using punky::obj::ObjectType;
using punky::tok::TokenType;

bool is_truthy(const Object& obj);
bool is_error(const Object& obj);

Object unknown_op_error(const Object& right);
Object unknown_op_error(const TokenType& op, const Object& right);
Object unknown_op_error(const TokenType& op, const Object& left, const Object& right);
Object type_mismatch_error(const TokenType& op, const Object& left, const Object& right);

Evaluator::Evaluator(std::unique_ptr<ast::Program> prog) :
  m_program{std::move(prog)}
{
}

Object Evaluator::interpret()
{
    return eval_program();
}

Object Evaluator::eval_program()
{
    Object result{};
    for (const auto& stmt : m_program->statements())
    {
        result = eval(*stmt);

        if (result.m_type == ObjectType::Return)
            return std::any_cast<Object>(std::get<std::any>(result.m_value));

        if (result.m_type == ObjectType::Error)
            return result;
    }
    return result;
}

Object Evaluator::eval(const ast::AstNode& node)
{
    switch (node.ast_type())
    {
        case AstType::ExpressionStmt:
            return eval(*node.expr_stmt()->expression());

        case AstType::BlockStmt:
            return eval_block_statements(*node.block_stmt());

        case AstType::ReturnStmt:
        {
            auto val = eval(*node.return_stmt()->ret_expr());
            return is_error(val) ? val
                                 : Object{ObjectType::Return, val};
        }

        case AstType::Int:
            return Object{ObjectType::Int, node.int_lit()->value()};

        case AstType::Bool:
            return Object{ObjectType::Boolean, node.boolean()->value()};

        case AstType::Prefix:
        {
            const auto right = eval(*node.prefix_expr()->right());
            return is_error(right) ? right
                                   : eval_prefix_expr(node.expr()->type(), right);
        }

        case AstType::Infix:
        {
            auto left = eval(*node.infix_expr()->left());
            if (is_error(left))
                return left;

            auto right = eval(*node.infix_expr()->right());
            if (is_error(right))
                return right;

            return eval_infix_expr(node.expr()->type(), left, right);
        }

        case AstType::If:
            return eval_if_expr(*node.if_expr());

        default:
            return Object{ObjectType::Null, std::monostate{}};
    }
}

Object Evaluator::eval_block_statements(const ast::BlockStmt& block)
{
    Object result{};
    for (const auto& stmt : block.statements())
    {
        result = eval(*stmt);

        if (result.m_type == ObjectType::Return || result.m_type == ObjectType::Error)
            return result;
    }
    return result;
}

Object Evaluator::eval_prefix_expr(const TokenType& op, const Object& right)
{
    switch (op)
    {
        case TokenType::Bang:
            return eval_bang_prefix_expr(right);

        case TokenType::Minus:
            return eval_minus_prefix_expr(right);

        default:
            return unknown_op_error(op, right);
    }
}

Object Evaluator::eval_bang_prefix_expr(const Object& right)
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

Object Evaluator::eval_minus_prefix_expr(const Object& right)
{
    if (right.m_type == ObjectType::Int)
        return Object{ObjectType::Int, -std::get<int>(right.m_value)};

    return unknown_op_error(right);
}

Object Evaluator::eval_infix_expr(const TokenType& op, const Object& left, const Object& right)
{
    if (left.m_type == ObjectType::Int && right.m_type == ObjectType::Int)
        return eval_int_infix_expr(op, left, right);

    if (left.m_type == ObjectType::Boolean && right.m_type == ObjectType::Boolean)
        return eval_bool_infix_expr(op, left, right);

    if (left.m_type != right.m_type)
        return type_mismatch_error(op, left, right);

    return unknown_op_error(op, left, right);
}

Object Evaluator::eval_int_infix_expr(const TokenType& op, const Object& left, const Object& right)
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
            return unknown_op_error(op, left, right);
    }
}

Object Evaluator::eval_bool_infix_expr(const TokenType& op, const Object& left, const Object& right)
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
            return unknown_op_error(op, left, right);
    }
}

Object Evaluator::eval_if_expr(const ast::IfExpression& if_expr)
{
    auto condition = eval(*if_expr.condition());

    if (is_error(condition))
        return condition;

    if (is_truthy(condition))
        return eval(*if_expr.consequence());

    if (if_expr.alternative())
        return eval(*if_expr.alternative());

    return Object{ObjectType::Null, std::monostate{}};
}

bool is_truthy(const Object& obj)
{
    switch (obj.m_type)
    {
        case ObjectType::Boolean:
            return std::get<bool>(obj.m_value);

        case ObjectType::Null:
            return false;

        default:
            return true;
    }
}

bool is_error(const Object& obj)
{
    return obj.m_type == ObjectType::Error;
}

Object unknown_op_error(const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("unknown operator: -" + obj::type_to_string(right.m_type))};
}

Object unknown_op_error(const TokenType& op, const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("unknown operator: " + type_to_string(op)
                              + obj::type_to_string(right.m_type))};
}

Object unknown_op_error(const TokenType& op, const Object& left, const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("unknown operator: " + obj::type_to_string(left.m_type))
                    + " " + type_to_string(op) + " " + obj::type_to_string(right.m_type)};
}

Object type_mismatch_error(const TokenType& op, const Object& left, const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("type mismatch: " + obj::type_to_string(left.m_type))
                    + " " + type_to_string(op) + " " + obj::type_to_string(right.m_type)};
}

}  // namespace punky::eval
