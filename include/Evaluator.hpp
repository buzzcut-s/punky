#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <memory>

#include "ast.hpp"

namespace punky
{
namespace obj
{
struct Object;
}
namespace tok
{
enum class TokenType;
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

    Object interpret();

private:
    std::unique_ptr<ast::Program> m_root;

    Object eval(ast::AstNode* node);

    Object eval_prefix_expr(const TokenType& op, const Object& right);
    Object eval_bang_prefix_expr(const Object& right);
    Object eval_minus_prefix_expr(const Object& right);

    Object eval_infix_expr(const TokenType& op,
                           const Object& left, const Object& right);
    Object eval_int_infix_expr(const TokenType& op,
                               const Object& left, const Object& right);
    Object eval_bool_infix_expr(const TokenType& op,
                                const Object& left, const Object& right);
};

}  // namespace punky::eval

#endif  // EVALUATOR_HPP
