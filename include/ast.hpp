#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Token.hpp"

namespace punky::ast
{

using punky::tok::Token;

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
      m_token(std::move(tok))
    {}

    [[nodiscard]] std::string token_literal() const final;
    [[nodiscard]] std::string to_string() const override = 0;

private:
    Token m_token;
};

using ExprNodePtr    = std::unique_ptr<ast::ExprNode>;
using ExprNodeVector = std::vector<ExprNodePtr>;

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
      m_token(std::move(tok))
    {}

    [[nodiscard]] std::string token_literal() const final;
    [[nodiscard]] std::string to_string() const override = 0;

private:
    Token m_token;
};

using StmtNodePtr    = std::unique_ptr<ast::StmtNode>;
using StmtNodeVector = std::vector<StmtNodePtr>;

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

    void push_stmt(std::unique_ptr<ast::StmtNode> stmt);

private:
    StmtNodeVector m_statements;
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

    [[nodiscard]] std::string to_string() const override;
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

    LetStmt(Token tok, Identifier name, ExprNodePtr value) :
      StmtNode{std::move(tok)},
      m_name(std::move(name)),
      m_value{std::move(value)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    Identifier  m_name;
    ExprNodePtr m_value;
};

class ReturnStmt : public StmtNode
{
public:
    ReturnStmt()                        = default;
    ReturnStmt(ReturnStmt const& other) = delete;
    ReturnStmt& operator=(ReturnStmt const& other) = delete;
    ReturnStmt(ReturnStmt&& other)                 = default;
    ReturnStmt& operator=(ReturnStmt&& other) = default;
    ~ReturnStmt() override                    = default;

    ReturnStmt(Token tok, ExprNodePtr ret_value) :
      StmtNode{std::move(tok)},
      m_ret_value{std::move(ret_value)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    ExprNodePtr m_ret_value;
};

class ExpressionStmt : public StmtNode
{
public:
    ExpressionStmt()                            = default;
    ExpressionStmt(ExpressionStmt const& other) = delete;
    ExpressionStmt& operator=(ExpressionStmt const& other) = delete;
    ExpressionStmt(ExpressionStmt&& other)                 = default;
    ExpressionStmt& operator=(ExpressionStmt&& other) = default;
    ~ExpressionStmt() override                        = default;

    ExpressionStmt(Token tok, ExprNodePtr expression) :
      StmtNode{std::move(tok)},
      m_expression{std::move(expression)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    ExprNodePtr m_expression;
};

class BlockStmt : public StmtNode
{
public:
    BlockStmt()                       = default;
    BlockStmt(BlockStmt const& other) = delete;
    BlockStmt& operator=(BlockStmt const& other) = delete;
    BlockStmt(BlockStmt&& other)                 = default;
    BlockStmt& operator=(BlockStmt&& other) = default;
    ~BlockStmt() override                   = default;

    explicit BlockStmt(Token tok) :
      StmtNode{std::move(tok)}
    {}

    void push_stmt(std::unique_ptr<ast::StmtNode> stmt);

    [[nodiscard]] std::string to_string() const override;

private:
    StmtNodeVector m_blk_statements;
};

class IntLiteral : public ExprNode
{
public:
    IntLiteral()                        = delete;
    IntLiteral(IntLiteral const& other) = default;
    IntLiteral& operator=(IntLiteral const& other) = default;
    IntLiteral(IntLiteral&& other)                 = default;
    IntLiteral& operator=(IntLiteral&& other) = default;
    ~IntLiteral() override                    = default;

    IntLiteral(Token tok, int int_value) :
      ExprNode{std::move(tok)},
      m_int_value(int_value)
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    int m_int_value;
};

class PrefixExpression : public ExprNode
{
public:
    PrefixExpression()                              = delete;
    PrefixExpression(PrefixExpression const& other) = delete;
    PrefixExpression& operator=(PrefixExpression const& other) = delete;
    PrefixExpression(PrefixExpression&& other)                 = default;
    PrefixExpression& operator=(PrefixExpression&& other) = default;
    ~PrefixExpression() override                          = default;

    PrefixExpression(Token tok, ExprNodePtr right) :
      ExprNode{std::move(tok)},
      m_right{std::move(right)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    ExprNodePtr m_right;
};

class InfixExpression : public ExprNode
{
public:
    InfixExpression()                             = delete;
    InfixExpression(InfixExpression const& other) = delete;
    InfixExpression& operator=(InfixExpression const& other) = delete;
    InfixExpression(InfixExpression&& other)                 = default;
    InfixExpression& operator=(InfixExpression&& other) = default;
    ~InfixExpression() override                         = default;

    InfixExpression(Token tok, ExprNodePtr left, ExprNodePtr right) :
      ExprNode{std::move(tok)},
      m_left{std::move(left)},
      m_right{std::move(right)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    ExprNodePtr m_left;
    ExprNodePtr m_right;
};

class Boolean : public ExprNode
{
public:
    Boolean()                     = delete;
    Boolean(Boolean const& other) = default;
    Boolean& operator=(Boolean const& other) = default;
    Boolean(Boolean&& other)                 = default;
    Boolean& operator=(Boolean&& other) = default;
    ~Boolean() override                 = default;

    Boolean(Token tok, bool bool_value) :
      ExprNode{std::move(tok)},
      m_bool_value(bool_value)
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    bool m_bool_value;
};

using OptIfAltBlk = std::optional<std::unique_ptr<ast::BlockStmt>>;

class IfExpression : public ExprNode
{
public:
    IfExpression()                          = delete;
    IfExpression(IfExpression const& other) = delete;
    IfExpression& operator=(IfExpression const& other) = delete;
    IfExpression(IfExpression&& other)                 = default;
    IfExpression& operator=(IfExpression&& other) = default;
    ~IfExpression() override                      = default;

    IfExpression(Token tok, ExprNodePtr condition,
                 std::unique_ptr<ast::BlockStmt> consequence,
                 OptIfAltBlk                      alternative) :
      ExprNode{std::move(tok)},
      m_condition{std::move(condition)},
      m_consequence{std::move(consequence)},
      m_alternative{std::move(alternative)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    ExprNodePtr                     m_condition;
    std::unique_ptr<ast::BlockStmt> m_consequence;
    OptIfAltBlk                      m_alternative;
};

using OptCallArgs = std::optional<std::unique_ptr<ExprNodeVector>>;

class CallExpression : public ExprNode
{
public:
    CallExpression()                            = delete;
    CallExpression(CallExpression const& other) = delete;
    CallExpression& operator=(CallExpression const& other) = delete;
    CallExpression(CallExpression&& other)                 = default;
    CallExpression& operator=(CallExpression&& other) = default;
    ~CallExpression() override                        = default;

    CallExpression(Token tok, ExprNodePtr function,
                   OptCallArgs arguments) :
      ExprNode{std::move(tok)},
      m_function{std::move(function)},
      m_arguments{std::move(arguments)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    ExprNodePtr m_function;
    OptCallArgs m_arguments;
};

using OptFnParams = std::optional<std::unique_ptr<std::vector<ast::Identifier>>>;

class FunctionLiteral : public ExprNode
{
public:
    FunctionLiteral()                             = delete;
    FunctionLiteral(FunctionLiteral const& other) = delete;
    FunctionLiteral& operator=(FunctionLiteral const& other) = delete;
    FunctionLiteral(FunctionLiteral&& other)                 = default;
    FunctionLiteral& operator=(FunctionLiteral&& other) = default;
    ~FunctionLiteral() override                         = default;

    FunctionLiteral(Token tok, OptFnParams params,
                    StmtNodePtr body) :
      ExprNode{std::move(tok)},
      m_params{std::move(params)},
      m_body{std::move(body)}
    {}

    [[nodiscard]] std::string to_string() const override;

private:
    OptFnParams m_params;
    StmtNodePtr m_body;
};

}  // namespace punky::ast

#endif  // AST_HPP
