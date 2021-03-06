#include "punky/Evaluator.hpp"

#include <memory>
#include <utility>
#include <vector>

#include <punky/Environment.hpp>
#include <punky/Object.hpp>
#include <punky/Token.hpp>
#include <punky/ast.hpp>

namespace punky::eval
{

using punky::ast::AstType;
using punky::obj::FunctionObject;
using punky::obj::Object;
using punky::obj::ObjectType;
using punky::tok::TokenType;

const Object Evaluator::M_TRUE_OBJ  = Object{ObjectType::Boolean, true};
const Object Evaluator::M_FALSE_OBJ = Object{ObjectType::Boolean, false};
const Object Evaluator::M_NULL_OBJ  = Object{ObjectType::Null, std::monostate{}};

static bool is_truthy(const Object& obj);
static bool is_error(const Object& obj);

static Object unknown_op_error(const Object& right);
static Object unknown_op_error(const TokenType& op, const Object& right);
static Object unknown_op_error(const TokenType& op, const Object& left, const Object& right);
static Object type_mismatch_error(const TokenType& op, const Object& left, const Object& right);
static Object unknown_ident_error(const ast::Identifier& ident);
static Object not_fn_error(const Object& not_fn);

static std::unique_ptr<env::Environment> extend_fn_env(const FunctionObject&      fn_obj,
                                                       const std::vector<Object>& args);

Evaluator::Evaluator(std::unique_ptr<ast::Program> prog) :
  m_program{std::move(prog)}
{
}

Object Evaluator::interpret(env::Environment& env) const
{
    return eval_program(env);
}

Object Evaluator::eval_program(env::Environment& env) const
{
    Object result{};
    for (const auto& stmt : m_program->statements())
    {
        result = eval(*stmt, env);

        if (result.m_type == ObjectType::Return)
            return std::any_cast<Object>(std::get<std::any>(result.m_value));

        if (result.m_type == ObjectType::Error)
            return result;
    }
    return result;
}

Object Evaluator::eval(const ast::AstNode& node, env::Environment& env)
{
    switch (node.ast_type())
    {
        case AstType::ExpressionStmt:
            return eval(*node.expr_stmt()->expression(), env);

        case AstType::BlockStmt:
            return eval_block_statements(*node.block_stmt(), env);

        case AstType::ReturnStmt:
        {
            auto val = eval(*node.return_stmt()->ret_expr(), env);
            return is_error(val) ? val
                                 : Object{ObjectType::Return, val};
        }

        case AstType::LetStmt:
        {
            auto val = eval(*node.let_stmt()->rhs(), env);
            if (is_error(val))
                return val;

            env.set(node.let_stmt()->lhs().name(), val);
            return Object{ObjectType::EmptyOut, std::monostate{}};
        }

        case AstType::Int:
            return Object{ObjectType::Int, node.int_lit()->value()};

        case AstType::Bool:
            return Object{ObjectType::Boolean, node.boolean()->value()};

        case AstType::Prefix:
        {
            auto right = eval(*node.prefix_expr()->right(), env);
            return is_error(right) ? right
                                   : eval_prefix_expr(node.expr()->type(), right);
        }

        case AstType::Infix:
        {
            auto left = eval(*node.infix_expr()->left(), env);
            if (is_error(left))
                return left;

            auto right = eval(*node.infix_expr()->right(), env);
            if (is_error(right))
                return right;

            return eval_infix_expr(node.expr()->type(), left, right);
        }

        case AstType::If:
            return eval_if_expr(*node.if_expr(), env);

        case AstType::Identifier:
            return eval_identifier(*node.identifier(), env);

        case AstType::Function:
        {
            // Fix this : This node.fn_lit() returned pointer is deleted at each repl.
            return Object{ObjectType::Function, FunctionObject{node.fn_lit(), &env}};
        }
        case AstType::Call:
        {
            auto fn = eval(*node.call_expr()->function(), env);
            if (is_error(fn))
                return fn;

            auto args = eval_expressions(node.call_expr()->arguments(), env);
            if (args.size() == 1 && is_error(args.front()))
                return args.front();

            return apply_function(fn, args);
        }

        default:
            return M_NULL_OBJ;
    }
}

Object Evaluator::eval_block_statements(const ast::BlockStmt& block, env::Environment& env)
{
    Object result{};
    for (const auto& stmt : block.statements())
    {
        result = eval(*stmt, env);

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
            return M_TRUE_OBJ;

        default:
            return M_FALSE_OBJ;
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

Object Evaluator::eval_if_expr(const ast::IfExpression& if_expr, env::Environment& env)
{
    auto condition = eval(*if_expr.condition(), env);

    if (is_error(condition))
        return condition;

    if (is_truthy(condition))
        return eval(*if_expr.consequence(), env);

    if (if_expr.alternative())
        return eval(*if_expr.alternative(), env);

    return M_NULL_OBJ;
}

Object Evaluator::eval_identifier(const ast::Identifier& ident, const env::Environment& env)
{
    if (auto val = env.get(ident.name()); val.has_value())
        return val.value();
    return unknown_ident_error(ident);
}

ObjectVector Evaluator::eval_expressions(const ast::ExprNodeVector* exprs, env::Environment& env)
{
    if (exprs)
    {
        std::vector<Object> result;
        for (const auto& expr : *exprs)
        {
            auto evaluated = eval(*expr, env);
            if (is_error(evaluated))
                return std::vector<Object>{evaluated};

            result.push_back(evaluated);
        }
        return result;
    }
    return {};
}

Object Evaluator::apply_function(const Object& fn, const ObjectVector& args)
{
    if (fn.m_type == ObjectType::Function)
    {
        auto fn_obj = std::get<FunctionObject>(fn.m_value);
        auto fn_env = extend_fn_env(fn_obj, args);
        auto value  = eval(*fn_obj.fn()->fn_lit()->body(), *fn_env);
        return value;
    }
    return not_fn_error(fn);
}

static std::unique_ptr<env::Environment> extend_fn_env(const FunctionObject&      fn_obj,
                                                       const std::vector<Object>& args)
{
    auto fn_env = std::make_unique<env::Environment>(fn_obj.env());

    const auto& node = *fn_obj.fn();  // Step into this to check if m_fn member is deleted.
    if (auto* params = node.fn_lit()->params(); params)
    {
        size_t i = 0;
        for (const auto& param : *params)
        {
            fn_env->set(param.name(), args[i++]);
        }
    }
    return fn_env;
}

static bool is_truthy(const Object& obj)
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

static bool is_error(const Object& obj)
{
    return obj.m_type == ObjectType::Error;
}

static Object unknown_op_error(const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("unknown operator: -" + obj::type_to_string(right.m_type))};
}

static Object unknown_op_error(const TokenType& op, const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("unknown operator: " + tok::type_to_string(op)
                              + obj::type_to_string(right.m_type))};
}

static Object unknown_op_error(const TokenType& op, const Object& left, const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("unknown operator: " + obj::type_to_string(left.m_type))
                    + " " + tok::type_to_string(op) + " " + obj::type_to_string(right.m_type)};
}

static Object type_mismatch_error(const TokenType& op, const Object& left, const Object& right)
{
    return Object{ObjectType::Error,
                  std::string("type mismatch: " + obj::type_to_string(left.m_type))
                    + " " + tok::type_to_string(op) + " " + obj::type_to_string(right.m_type)};
}

static Object unknown_ident_error(const ast::Identifier& ident)
{
    return Object{ObjectType::Error,
                  std::string("identifier not found: " + ident.name())};
}

static Object not_fn_error(const Object& not_fn)
{
    return Object{ObjectType::Error,
                  std::string("not a function: " + obj::type_to_string(not_fn.m_type))};
}

}  // namespace punky::eval
