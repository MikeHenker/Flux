#pragma once
#include <memory>
#include <vector>
#include <string>
#include <variant>

// Forward declarations
class Visitor;
class FluxCallable;

// Base AST node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// Value type for Flux
using FluxValue = std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<FluxCallable>>;

// Expression nodes
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class LiteralExpression : public Expression {
public:
    FluxValue value;
    
    LiteralExpression(FluxValue val) : value(val) {}
    void accept(Visitor& visitor) override;
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    
    IdentifierExpression(const std::string& n) : name(n) {}
    void accept(Visitor& visitor) override;
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string operator_;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(std::unique_ptr<Expression> l, const std::string& op, std::unique_ptr<Expression> r)
        : left(std::move(l)), operator_(op), right(std::move(r)) {}
    void accept(Visitor& visitor) override;
};

class UnaryExpression : public Expression {
public:
    std::string operator_;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(const std::string& op, std::unique_ptr<Expression> expr)
        : operator_(op), operand(std::move(expr)) {}
    void accept(Visitor& visitor) override;
};

class CallExpression : public Expression {
public:
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpression(std::unique_ptr<Expression> c, std::vector<std::unique_ptr<Expression>> args)
        : callee(std::move(c)), arguments(std::move(args)) {}
    void accept(Visitor& visitor) override;
};

// Statement nodes
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    ExpressionStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
    void accept(Visitor& visitor) override;
};

class VarDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> initializer;
    
    VarDeclaration(const std::string& n, std::unique_ptr<Expression> init)
        : name(n), initializer(std::move(init)) {}
    void accept(Visitor& visitor) override;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    BlockStatement(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}
    void accept(Visitor& visitor) override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
    
    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> thenStmt, std::unique_ptr<Statement> elseStmt = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenStmt)), elseBranch(std::move(elseStmt)) {}
    void accept(Visitor& visitor) override;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    
    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void accept(Visitor& visitor) override;
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockStatement> body;
    
    FunctionDeclaration(const std::string& n, std::vector<std::string> params, std::unique_ptr<BlockStatement> b)
        : name(n), parameters(std::move(params)), body(std::move(b)) {}
    void accept(Visitor& visitor) override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    ReturnStatement(std::unique_ptr<Expression> val) : value(std::move(val)) {}
    void accept(Visitor& visitor) override;
};

class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    PrintStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
    void accept(Visitor& visitor) override;
};

// Program (root node)
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    Program(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}
    void accept(Visitor& visitor) override;
};

// Visitor interface
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(LiteralExpression& node) = 0;
    virtual void visit(IdentifierExpression& node) = 0;
    virtual void visit(BinaryExpression& node) = 0;
    virtual void visit(UnaryExpression& node) = 0;
    virtual void visit(CallExpression& node) = 0;
    virtual void visit(ExpressionStatement& node) = 0;
    virtual void visit(VarDeclaration& node) = 0;
    virtual void visit(BlockStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(PrintStatement& node) = 0;
    virtual void visit(Program& node) = 0;
};