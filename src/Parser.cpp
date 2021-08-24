#include "../include/Parser.hpp"

#include <memory>
#include <utility>

#include "../include/Lexer.hpp"
#include "../include/ast.hpp"

namespace parser
{

Parser::Parser(Lexer lex) :
  m_lex{std::move(lex)}
{
    next_token();
    next_token();
}

void Parser::next_token()
{
    m_curr_tok = m_peek_tok;
    m_peek_tok = m_lex.next_token();
}

auto Parser::parse_program() -> std::unique_ptr<ast::Program>
{
    return nullptr;
}

}  // namespace parser
