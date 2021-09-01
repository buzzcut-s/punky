#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <memory>
#include <vector>

#include "Object.hpp"
#include "ast.hpp"

namespace punky
{
namespace tok
{
enum class TokenType;
}
namespace env
{
class Environment;
}
}  // namespace punky

namespace punky::eval
{

using punky::obj::Object;
using punky::tok::TokenType;

class Evaluator
{
public:
    explicit Evaluator(std::unique_ptr<ast::Program> prog);

    [[nodiscard]] Object interpret(env::Environment& env) const;

private:
    using Expressions = std::vector<std::unique_ptr<punky::ast::ExprNode>>;

    std::unique_ptr<ast::Program> m_program;

    static const Object M_TRUE_OBJ;
    static const Object M_FALSE_OBJ;
    static const Object M_NULL_OBJ;

    [[nodiscard]] Object eval_program(env::Environment& env) const;

    static Object eval(const ast::AstNode& node, env::Environment& env);

    static Object eval_block_statements(const ast::BlockStmt& block, env::Environment& env);

    static Object eval_prefix_expr(const TokenType& op, const Object& right);
    static Object eval_bang_prefix_expr(const Object& right);
    static Object eval_minus_prefix_expr(const Object& right);

    static Object eval_infix_expr(const TokenType& op, const Object& left, const Object& right);
    static Object eval_int_infix_expr(const TokenType& op, const Object& left, const Object& right);
    static Object eval_bool_infix_expr(const TokenType& op, const Object& left, const Object& right);

    static Object eval_if_expr(const ast::IfExpression& if_expr, env::Environment& env);
    static Object eval_identifier(const ast::Identifier& ident, env::Environment& env);

    static std::vector<Object> eval_expressions(const Expressions& exprs, env::Environment& env);

    static Object apply_function(const Object& fn, const std::vector<Object>& args);

    static std::unique_ptr<env::Environment> extend_fn_env(const Object& fn, std::vector<Object>& args);
};

}  // namespace punky::eval

#endif  // EVALUATOR_HPP
