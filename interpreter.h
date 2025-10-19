#pragma once
#include "ast.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

// Forward declaration
class FluxFunction;
class ReturnException;

// Environment for variable and function storage
class Environment {
public:
    Environment(std::shared_ptr<Environment> parent = nullptr);
    
    void define(const std::string& name, FluxValue value);
    FluxValue get(const std::string& name);
    void assign(const std::string& name, FluxValue value);
    
private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, FluxValue> values;
};

// Exception for return statements
class ReturnException : public std::exception {
public:
    FluxValue value;
    ReturnException(FluxValue val) : value(val) {}
};

// Callable interface for functions
class FluxCallable {
public:
    virtual ~FluxCallable() = default;
    virtual int arity() const = 0;
    virtual FluxValue call(class Interpreter& interpreter, const std::vector<FluxValue>& arguments) = 0;
    virtual std::string toString() const = 0;
};

// User-defined function
class FluxFunction : public FluxCallable {
public:
    FunctionDeclaration* declaration;
    std::shared_ptr<Environment> closure;
    
    FluxFunction(FunctionDeclaration* decl, std::shared_ptr<Environment> closure);
    
    int arity() const override;
    FluxValue call(Interpreter& interpreter, const std::vector<FluxValue>& arguments) override;
    std::string toString() const override;
};

// Native function
class NativeFunction : public FluxCallable {
public:
    std::function<FluxValue(const std::vector<FluxValue>&)> function;
    int paramCount;
    std::string name;
    
    NativeFunction(const std::string& n, int params, std::function<FluxValue(const std::vector<FluxValue>&)> func);
    
    int arity() const override;
    FluxValue call(Interpreter& interpreter, const std::vector<FluxValue>& arguments) override;
    std::string toString() const override;
};

// Main interpreter class
class Interpreter : public Visitor {
public:
    Interpreter();
    
    void interpret(Program& program);
    void executeBlock(const std::vector<std::unique_ptr<Statement>>& statements, std::shared_ptr<Environment> environment);
    
    // Visitor methods
    void visit(LiteralExpression& node) override;
    void visit(IdentifierExpression& node) override;
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(CallExpression& node) override;
    void visit(ExpressionStatement& node) override;
    void visit(VarDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(ReturnStatement& node) override;
    void visit(PrintStatement& node) override;
    void visit(Program& node) override;
    
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    
private:
    FluxValue lastValue;
    
    FluxValue evaluate(Expression* expr);
    void execute(Statement* stmt);
    bool isTruthy(FluxValue value);
    bool isEqual(FluxValue left, FluxValue right);
    std::string stringify(FluxValue value);
    void checkNumberOperand(const std::string& op, FluxValue operand);
    void checkNumberOperands(const std::string& op, FluxValue left, FluxValue right);
    
    void defineNativeFunctions();
};