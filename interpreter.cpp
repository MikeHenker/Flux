#include "interpreter.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <chrono>

// Environment implementation
Environment::Environment(std::shared_ptr<Environment> parent) : enclosing(parent) {}

void Environment::define(const std::string& name, FluxValue value) {
    values[name] = value;
}

FluxValue Environment::get(const std::string& name) {
    auto it = values.find(name);
    if (it != values.end()) {
        return it->second;
    }
    
    if (enclosing) {
        return enclosing->get(name);
    }
    
    throw std::runtime_error("Undefined variable '" + name + "'");
}

void Environment::assign(const std::string& name, FluxValue value) {
    auto it = values.find(name);
    if (it != values.end()) {
        it->second = value;
        return;
    }
    
    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }
    
    throw std::runtime_error("Undefined variable '" + name + "'");
}

// FluxFunction implementation
FluxFunction::FluxFunction(FunctionDeclaration* decl, std::shared_ptr<Environment> closure) 
    : declaration(decl), closure(closure) {}

int FluxFunction::arity() const {
    return declaration->parameters.size();
}

FluxValue FluxFunction::call(Interpreter& interpreter, const std::vector<FluxValue>& arguments) {
    auto environment = std::make_shared<Environment>(closure);
    
    for (size_t i = 0; i < declaration->parameters.size(); i++) {
        environment->define(declaration->parameters[i], arguments[i]);
    }
    
    try {
        interpreter.executeBlock(declaration->body->statements, environment);
    } catch (const ReturnException& returnValue) {
        return returnValue.value;
    }
    
    return nullptr;
}

std::string FluxFunction::toString() const {
    return "<fn " + declaration->name + ">";
}

// NativeFunction implementation
NativeFunction::NativeFunction(const std::string& n, int params, std::function<FluxValue(const std::vector<FluxValue>&)> func) 
    : name(n), paramCount(params), function(func) {}

int NativeFunction::arity() const {
    return paramCount;
}

FluxValue NativeFunction::call(Interpreter& interpreter, const std::vector<FluxValue>& arguments) {
    return function(arguments);
}

std::string NativeFunction::toString() const {
    return "<native fn " + name + ">";
}

// Interpreter implementation
Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
    defineNativeFunctions();
}

void Interpreter::defineNativeFunctions() {
    // Clock function
    globals->define("clock", std::make_shared<NativeFunction>("clock", 0, 
        [](const std::vector<FluxValue>&) -> FluxValue {
            auto now = std::chrono::high_resolution_clock::now();
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            return static_cast<double>(time.count()) / 1000.0;
        }));
    
    // Mathematical functions
    globals->define("sqrt", std::make_shared<NativeFunction>("sqrt", 1,
        [](const std::vector<FluxValue>& args) -> FluxValue {
            if (auto num = std::get_if<double>(&args[0])) {
                return std::sqrt(*num);
            }
            throw std::runtime_error("sqrt() requires a number argument");
        }));
        
    globals->define("abs", std::make_shared<NativeFunction>("abs", 1,
        [](const std::vector<FluxValue>& args) -> FluxValue {
            if (auto num = std::get_if<double>(&args[0])) {
                return std::abs(*num);
            }
            throw std::runtime_error("abs() requires a number argument");
        }));
}

void Interpreter::interpret(Program& program) {
    try {
        program.accept(*this);
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

void Interpreter::executeBlock(const std::vector<std::unique_ptr<Statement>>& statements, std::shared_ptr<Environment> env) {
    auto previous = environment;
    environment = env;
    
    try {
        for (const auto& statement : statements) {
            execute(statement.get());
        }
    } catch (...) {
        environment = previous;
        throw;
    }
    
    environment = previous;
}

FluxValue Interpreter::evaluate(Expression* expr) {
    expr->accept(*this);
    return lastValue;
}

void Interpreter::execute(Statement* stmt) {
    stmt->accept(*this);
}

bool Interpreter::isTruthy(FluxValue value) {
    if (std::holds_alternative<std::nullptr_t>(value)) return false;
    if (auto b = std::get_if<bool>(&value)) return *b;
    return true;
}

bool Interpreter::isEqual(FluxValue left, FluxValue right) {
    return left == right;
}

std::string Interpreter::stringify(FluxValue value) {
    if (std::holds_alternative<std::nullptr_t>(value)) {
        return "nil";
    }
    if (auto str = std::get_if<std::string>(&value)) {
        return *str;
    }
    if (auto num = std::get_if<double>(&value)) {
        std::ostringstream oss;
        oss << *num;
        return oss.str();
    }
    if (auto b = std::get_if<bool>(&value)) {
        return *b ? "true" : "false";
    }
    if (auto callable = std::get_if<std::shared_ptr<FluxCallable>>(&value)) {
        return (*callable)->toString();
    }
    return "unknown";
}

void Interpreter::checkNumberOperand(const std::string& op, FluxValue operand) {
    if (!std::holds_alternative<double>(operand)) {
        throw std::runtime_error("Operand must be a number for " + op);
    }
}

void Interpreter::checkNumberOperands(const std::string& op, FluxValue left, FluxValue right) {
    if (!std::holds_alternative<double>(left) || !std::holds_alternative<double>(right)) {
        throw std::runtime_error("Operands must be numbers for " + op);
    }
}

// Visitor methods
void Interpreter::visit(LiteralExpression& node) {
    lastValue = node.value;
}

void Interpreter::visit(IdentifierExpression& node) {
    lastValue = environment->get(node.name);
}

void Interpreter::visit(BinaryExpression& node) {
    FluxValue left = evaluate(node.left.get());
    FluxValue right = evaluate(node.right.get());
    
    if (node.operator_ == "=") {
        if (auto identifier = dynamic_cast<IdentifierExpression*>(node.left.get())) {
            environment->assign(identifier->name, right);
            lastValue = right;
            return;
        }
        throw std::runtime_error("Invalid assignment target");
    }
    
    if (node.operator_ == "+") {
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            lastValue = std::get<double>(left) + std::get<double>(right);
            return;
        }
        if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
            lastValue = std::get<std::string>(left) + std::get<std::string>(right);
            return;
        }
        throw std::runtime_error("Operands must be two numbers or two strings");
    }
    
    if (node.operator_ == "-") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::get<double>(left) - std::get<double>(right);
        return;
    }
    
    if (node.operator_ == "*") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::get<double>(left) * std::get<double>(right);
        return;
    }
    
    if (node.operator_ == "/") {
        checkNumberOperands(node.operator_, left, right);
        double rightVal = std::get<double>(right);
        if (rightVal == 0) throw std::runtime_error("Division by zero");
        lastValue = std::get<double>(left) / rightVal;
        return;
    }
    
    if (node.operator_ == "%") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::fmod(std::get<double>(left), std::get<double>(right));
        return;
    }
    
    if (node.operator_ == ">") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::get<double>(left) > std::get<double>(right);
        return;
    }
    
    if (node.operator_ == ">=") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::get<double>(left) >= std::get<double>(right);
        return;
    }
    
    if (node.operator_ == "<") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::get<double>(left) < std::get<double>(right);
        return;
    }
    
    if (node.operator_ == "<=") {
        checkNumberOperands(node.operator_, left, right);
        lastValue = std::get<double>(left) <= std::get<double>(right);
        return;
    }
    
    if (node.operator_ == "!=") {
        lastValue = !isEqual(left, right);
        return;
    }
    
    if (node.operator_ == "==") {
        lastValue = isEqual(left, right);
        return;
    }
    
    if (node.operator_ == "and") {
        if (!isTruthy(left)) {
            lastValue = left;
        } else {
            lastValue = right;
        }
        return;
    }
    
    if (node.operator_ == "or") {
        if (isTruthy(left)) {
            lastValue = left;
        } else {
            lastValue = right;
        }
        return;
    }
    
    throw std::runtime_error("Unknown binary operator: " + node.operator_);
}

void Interpreter::visit(UnaryExpression& node) {
    FluxValue right = evaluate(node.operand.get());
    
    if (node.operator_ == "-") {
        checkNumberOperand(node.operator_, right);
        lastValue = -std::get<double>(right);
        return;
    }
    
    if (node.operator_ == "not" || node.operator_ == "!") {
        lastValue = !isTruthy(right);
        return;
    }
    
    throw std::runtime_error("Unknown unary operator: " + node.operator_);
}

void Interpreter::visit(CallExpression& node) {
    FluxValue callee = evaluate(node.callee.get());
    
    std::vector<FluxValue> arguments;
    for (const auto& arg : node.arguments) {
        arguments.push_back(evaluate(arg.get()));
    }
    
    auto callable = std::get_if<std::shared_ptr<FluxCallable>>(&callee);
    if (!callable) {
        throw std::runtime_error("Can only call functions");
    }
    
    if (arguments.size() != (*callable)->arity()) {
        throw std::runtime_error("Expected " + std::to_string((*callable)->arity()) + 
                                " arguments but got " + std::to_string(arguments.size()));
    }
    
    lastValue = (*callable)->call(*this, arguments);
}

void Interpreter::visit(ExpressionStatement& node) {
    evaluate(node.expression.get());
}

void Interpreter::visit(VarDeclaration& node) {
    FluxValue value = nullptr;
    if (node.initializer) {
        value = evaluate(node.initializer.get());
    }
    environment->define(node.name, value);
}

void Interpreter::visit(BlockStatement& node) {
    executeBlock(node.statements, std::make_shared<Environment>(environment));
}

void Interpreter::visit(IfStatement& node) {
    FluxValue condition = evaluate(node.condition.get());
    
    if (isTruthy(condition)) {
        execute(node.thenBranch.get());
    } else if (node.elseBranch) {
        execute(node.elseBranch.get());
    }
}

void Interpreter::visit(WhileStatement& node) {
    while (isTruthy(evaluate(node.condition.get()))) {
        execute(node.body.get());
    }
}

void Interpreter::visit(FunctionDeclaration& node) {
    auto function = std::make_shared<FluxFunction>(&node, environment);
    environment->define(node.name, function);
}

void Interpreter::visit(ReturnStatement& node) {
    FluxValue value = nullptr;
    if (node.value) {
        value = evaluate(node.value.get());
    }
    throw ReturnException(value);
}

void Interpreter::visit(PrintStatement& node) {
    FluxValue value = evaluate(node.expression.get());
    std::cout << stringify(value) << std::endl;
}

void Interpreter::visit(Program& node) {
    for (const auto& statement : node.statements) {
        execute(statement.get());
    }
}