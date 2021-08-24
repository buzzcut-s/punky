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
        let_str.append(m_value->to_string());
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

}  // namespace ast
