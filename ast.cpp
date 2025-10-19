#include "ast.h"

// Expression accept methods
void LiteralExpression::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void IdentifierExpression::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpression::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void CallExpression::accept(Visitor& visitor) {
    visitor.visit(*this);
}

// Statement accept methods
void ExpressionStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void VarDeclaration::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void BlockStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void IfStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void WhileStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void PrintStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Program::accept(Visitor& visitor) {
    visitor.visit(*this);
}