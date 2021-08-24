#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Token.hpp"

namespace ast
{

struct AstNode
{
    AstNode()                     = default;
    AstNode(AstNode const& other) = default;
    AstNode& operator=(AstNode const& other) = default;
    AstNode(AstNode&& other)                 = default;
    AstNode& operator=(AstNode&& other) = default;
    virtual ~AstNode()                  = default;

    [[nodiscard]] virtual std::string token_literal() const = 0;
    [[nodiscard]] virtual std::string to_string() const     = 0;
};

class ExprNode : public AstNode
{
public:
    ExprNode()                      = default;
    ExprNode(ExprNode const& other) = default;
    ExprNode& operator=(ExprNode const& other) = default;
    ExprNode(ExprNode&& other)                 = default;
    ExprNode& operator=(ExprNode&& other) = default;
    ~ExprNode() override                  = default;

    explicit ExprNode(Token tok) :
      m_token(std::move(tok)) {}

    [[nodiscard]] std::string token_literal() const final;
    [[nodiscard]] std::string to_string() const override = 0;

private:
    Token m_token;
};

class StmtNode : public AstNode
{
public:
    StmtNode()                      = default;
    StmtNode(StmtNode const& other) = default;
    StmtNode& operator=(StmtNode const& other) = default;
    StmtNode(StmtNode&& other)                 = default;
    StmtNode& operator=(StmtNode&& other) = default;
    ~StmtNode() override                  = default;

    explicit StmtNode(Token tok) :
      m_token(std::move(tok)) {}

    [[nodiscard]] std::string token_literal() const final;
    [[nodiscard]] std::string to_string() const override = 0;

private:
    Token m_token;
};

class Program : public AstNode
{
public:
    Program()                     = default;
    Program(Program const& other) = delete;
    Program& operator=(Program const& other) = delete;
    Program(Program&& other)                 = default;
    Program& operator=(Program&& other) = default;
    ~Program() override                 = default;

    [[nodiscard]] std::string token_literal() const override;
    [[nodiscard]] std::string to_string() const override;

private:
    std::vector<std::unique_ptr<StmtNode>> m_statements;
};

struct Identifier : public ExprNode
{
    Identifier()                        = default;
    Identifier(Identifier const& other) = default;
    Identifier& operator=(Identifier const& other) = default;
    Identifier(Identifier&& other)                 = default;
    Identifier& operator=(Identifier&& other) = default;
    ~Identifier() override                    = default;

    explicit Identifier(Token tok) :
      ExprNode{std::move(tok)}
    {}

    [[nodiscard]] std::string to_string() const override { return token_literal(); }
};

class LetStmt : public StmtNode
{
public:
    LetStmt()                     = default;
    LetStmt(LetStmt const& other) = delete;
    LetStmt& operator=(LetStmt const& other) = delete;
    LetStmt(LetStmt&& other)                 = default;
    LetStmt& operator=(LetStmt&& other) = default;
    ~LetStmt() override                 = default;

    explicit LetStmt(Token tok) :
      StmtNode{std::move(tok)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    Identifier                m_name;
    std::unique_ptr<ExprNode> m_value;
};

}  // namespace ast

#endif  // AST_HPP
