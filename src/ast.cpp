#include "../include/ast.hpp"

#include <string>

namespace punky::ast
{

ExprNode* AstNode::expr()
{
    return static_cast<ExprNode*>(this);
}

StmtNode* AstNode::stmt()
{
    return static_cast<StmtNode*>(this);
}

const ExprNode* AstNode::expr() const
{
    return static_cast<const ExprNode*>(this);
}

const StmtNode* AstNode::stmt() const
{
    return static_cast<const StmtNode*>(this);
}

IntLiteral* AstNode::int_lit()
{
    return static_cast<IntLiteral*>(this);
}

Boolean* AstNode::boolean()
{
    return static_cast<Boolean*>(this);
}

PrefixExpression* AstNode::prefix_expr()
{
    return static_cast<PrefixExpression*>(this);
}

InfixExpression* AstNode::infix_expr()
{
    return static_cast<InfixExpression*>(this);
}

IfExpression* AstNode::if_expr()
{
    return static_cast<IfExpression*>(this);
}

const IntLiteral* AstNode::int_lit() const
{
    return static_cast<const IntLiteral*>(this);
}

const Boolean* AstNode::boolean() const
{
    return static_cast<const Boolean*>(this);
}

const PrefixExpression* AstNode::prefix_expr() const
{
    return static_cast<const PrefixExpression*>(this);
}

const InfixExpression* AstNode::infix_expr() const
{
    return static_cast<const InfixExpression*>(this);
}

const IfExpression* AstNode::if_expr() const
{
    return static_cast<const IfExpression*>(this);
}

ExpressionStmt* AstNode::expr_stmt()
{
    return static_cast<ExpressionStmt*>(this);
}

BlockStmt* AstNode::block_stmt()
{
    return static_cast<BlockStmt*>(this);
}

ReturnStmt* AstNode::return_stmt()
{
    return static_cast<ReturnStmt*>(this);
}

const ExpressionStmt* AstNode::expr_stmt() const
{
    return static_cast<const ExpressionStmt*>(this);
}

const BlockStmt* AstNode::block_stmt() const
{
    return static_cast<const BlockStmt*>(this);
}

const ReturnStmt* AstNode::return_stmt() const
{
    return static_cast<const ReturnStmt*>(this);
}

std::string ExprNode::token_literal() const
{
    return m_token.m_literal.has_value() ? m_token.m_literal.value()
                                         : type_to_string(m_token.m_type);
}

std::string StmtNode::token_literal() const
{
    return m_token.m_literal.has_value() ? m_token.m_literal.value()
                                         : type_to_string(m_token.m_type);
}

std::string Program::token_literal() const
{
    return m_statements.empty() ? ""
                                : m_statements[0]->token_literal();
}

std::string Program::to_string() const
{
    std::string prog_str;
    for (const auto& stmt : m_statements)
        prog_str.append(stmt->to_string() + "\n");
    return prog_str;
}

void Program::push_stmt(std::unique_ptr<ast::StmtNode> stmt)
{
    m_statements.push_back(std::move(stmt));
}

std::string LetStmt::to_string() const
{
    return m_value ? token_literal() + " " + m_name.to_string() + " = " + m_value->to_string()
                   : token_literal() + " " + m_name.to_string();
}

std::string Identifier::to_string() const
{
    return token_literal();
}

std::string ReturnStmt::to_string() const
{
    return m_ret_expr ? token_literal() + " " + m_ret_expr->to_string()
                      : token_literal() + " ";
}

std::string ExpressionStmt::to_string() const
{
    return m_expression ? m_expression->to_string()
                        : "";
}

void BlockStmt::push_stmt(std::unique_ptr<ast::StmtNode> stmt)
{
    m_blk_statements.push_back(std::move(stmt));
}

std::string BlockStmt::to_string() const
{
    std::string blk_str{"{ "};
    for (const auto& stmt : m_blk_statements)
        blk_str.append(stmt->to_string() + "\n");
    blk_str.pop_back();
    blk_str.append(" }");
    return blk_str;
}

std::string IntLiteral::to_string() const
{
    return std::to_string(m_int_value);
}

std::string PrefixExpression::to_string() const
{
    return m_right ? "(" + token_literal() + " " + m_right->to_string() + ")"
                   : "";
}

std::string InfixExpression::to_string() const
{
    return (m_left && m_right) ? "(" + m_left->to_string()
                                   + " " + token_literal() + " "
                                   + m_right->to_string() + ")"
                               : "";
}

std::string Boolean::to_string() const
{
    return token_literal();
}

std::string IfExpression::to_string() const
{
    if (m_condition && m_consequence)
    {
        std::string if_str{token_literal() + " " + m_condition->to_string()
                           + " " + m_consequence->to_string()};
        if (m_alternative.has_value())
            if_str.append("else " + m_alternative.value()->to_string());
        return if_str;
    }
    return "";
}

std::string CallExpression::to_string() const
{
    std::string call_str;
    if (m_function)
    {
        call_str.append(m_function->to_string() + token_literal());
        if (m_arguments.has_value())
        {
            std::string args_str;
            for (const auto& args : *m_arguments.value())
                args_str.append(args->to_string() + ", ");
            if (args_str.size() > 2)
            {
                args_str.pop_back();
                args_str.pop_back();
            }
            call_str.append(args_str + ")");
        }
        else
            call_str.append(")");
    }
    return call_str;
}

std::string FunctionLiteral::to_string() const
{
    std::string fn_str{token_literal() + "("};
    if (m_body)
    {
        if (m_params.has_value())
        {
            std::string param_str;
            for (const auto& param : *m_params.value())
                param_str.append(param.to_string() + ", ");
            if (param_str.size() > 2)
            {
                param_str.pop_back();
                param_str.pop_back();
            }
            fn_str.append(param_str + ") " + m_body->to_string());
        }
        else
            fn_str.append(") " + m_body->to_string());
    }
    return fn_str;
}

}  // namespace punky::ast
