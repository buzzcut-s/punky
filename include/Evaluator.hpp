#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <memory>

#include "Object.hpp"
#include "ast.hpp"

namespace punky::tok
{
enum class TokenType;
}

namespace punky::eval
{

using punky::obj::Object;
using punky::obj::ObjectType;
using punky::tok::TokenType;

class Evaluator
{
public:
    explicit Evaluator(std::unique_ptr<ast::Program> prog);

    Object interpret();

private:
    std::unique_ptr<ast::Program> m_program;

    const Object m_true_obj  = Object{ObjectType::Boolean, true};
    const Object m_false_obj = Object{ObjectType::Boolean, false};
    const Object m_null_obj  = Object{ObjectType::Null, std::monostate{}};

    Object eval_program();

    Object eval(const ast::AstNode& node);

    Object eval_block_statements(const ast::BlockStmt& block);

    Object eval_prefix_expr(const TokenType& op, const Object& right);
    Object eval_bang_prefix_expr(const Object& right);
    Object eval_minus_prefix_expr(const Object& right);

    Object eval_infix_expr(const TokenType& op, const Object& left, const Object& right);
    Object eval_int_infix_expr(const TokenType& op, const Object& left, const Object& right);
    Object eval_bool_infix_expr(const TokenType& op, const Object& left, const Object& right);

    Object eval_if_expr(const ast::IfExpression& if_expr);
};

}  // namespace punky::eval

#endif  // EVALUATOR_HPP
