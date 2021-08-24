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
    for (const auto& smt : m_statements)
        prog_str.append(smt->to_string() + "\n");
    return prog_str;
}

std::string LetStmt::to_string() const
{
    std::string let_str{token_literal()};
    let_str.append(m_name.to_string());
    if (m_value)
        let_str.append(m_value->to_string());
    let_str.append(";\n");
    return let_str;
}

}  // namespace ast
