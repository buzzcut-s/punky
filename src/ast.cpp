#include "../include/ast.hpp"

#include <string>

namespace ast
{

std::string ExprNode::token_literal() const
{
    if (m_token.m_literal.has_value())
        return m_token.m_literal.value();
    return type_to_string(m_token.m_type);
}

std::string StmtNode::token_literal() const
{
    if (m_token.m_literal.has_value())
        return m_token.m_literal.value();
    return type_to_string(m_token.m_type);
}

std::string Program::token_literal() const
{
    if (!m_statements.empty())
        return m_statements[0]->token_literal();
    return "";
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
    std::string let_str{token_literal() + " "};
    let_str.append(m_name.to_string());
    if (m_value)
        let_str.append(" = " + m_value->to_string());
    return let_str;
}

std::string Identifier::to_string() const
{
    return token_literal();
}

std::string ReturnStmt::to_string() const
{
    std::string ret_str{token_literal() + " "};
    if (m_ret_value)
        ret_str.append(m_ret_value->to_string());
    return ret_str;
}

std::string ExpressionStmt::to_string() const
{
    if (m_expression)
        return m_expression->to_string();
    return "";
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
    if (m_right)
        return "(" + token_literal() + " " + m_right->to_string() + ")";
    return "";
}

std::string InfixExpression::to_string() const
{
    if (m_left && m_right)
        return "(" + m_left->to_string()
               + " " + token_literal() + " "
               + m_right->to_string() + ")";
    return "";
}

std::string Boolean::to_string() const
{
    return token_literal();
}

std::string IfExpression::to_string() const
{
    std::string if_str;
    if (m_condition && m_consequence)
    {
        if_str.append(token_literal() + " " + m_condition->to_string()
                      + " " + m_consequence->to_string());
        if (m_alternative)
            if_str.append("else " + m_alternative->to_string());
    }
    return if_str;
}

std::string CallExpression::to_string() const
{
    std::string call_str;
    if (m_function)
    {
        call_str.append(m_function->to_string() + token_literal());
        std::string args_str;
        for (const auto& args : *m_arguments)
            args_str.append(args->to_string() + ", ");
        if (args_str.size() > 2)
        {
            args_str.pop_back();
            args_str.pop_back();
        }
        call_str.append(args_str + ")");
    }
    return call_str;
}

std::string FunctionLiteral::to_string() const
{
    std::string fn_str;
    if (m_params && m_body)
    {
        fn_str.append(token_literal() + "(");
        std::string param_str;
        for (const auto& param : *m_params)
            param_str.append(param.to_string() + ", ");
        if (param_str.size() > 2)
        {
            param_str.pop_back();
            param_str.pop_back();
        }
        fn_str.append(param_str + ") " + m_body->to_string());
    }
    return fn_str;
}

}  // namespace ast
