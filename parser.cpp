#include "parser.h"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::unique_ptr<Program> Parser::parse() {
    return program();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::FUN:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
            case TokenType::PRINT:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<Program> Parser::program() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!isAtEnd()) {
        // Skip newlines at top level
        if (match({TokenType::NEWLINE})) continue;
        
        try {
            auto stmt = declaration();
            if (stmt) statements.push_back(std::move(stmt));
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return std::make_unique<Program>(std::move(statements));
}

std::unique_ptr<Statement> Parser::declaration() {
    if (match({TokenType::LET})) return varDeclaration();
    if (match({TokenType::FUN})) return functionDeclaration();
    return statement();
}

std::unique_ptr<VarDeclaration> Parser::varDeclaration() {
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable name");
        return nullptr;
    }
    
    std::string name = advance().lexeme;
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match({TokenType::ASSIGN})) {
        initializer = expression();
    }
    
    match({TokenType::SEMICOLON, TokenType::NEWLINE});
    return std::make_unique<VarDeclaration>(name, std::move(initializer));
}

std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected function name");
        return nullptr;
    }
    
    std::string name = advance().lexeme;
    
    if (!match({TokenType::LEFT_PAREN})) {
        error("Expected '(' after function name");
        return nullptr;
    }
    
    std::vector<std::string> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (!check(TokenType::IDENTIFIER)) {
                error("Expected parameter name");
                return nullptr;
            }
            parameters.push_back(advance().lexeme);
        } while (match({TokenType::COMMA}));
    }
    
    if (!match({TokenType::RIGHT_PAREN})) {
        error("Expected ')' after parameters");
        return nullptr;
    }
    
    auto body = blockStatement();
    return std::make_unique<FunctionDeclaration>(name, std::move(parameters), std::move(body));
}

std::unique_ptr<Statement> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::LEFT_BRACE})) return blockStatement();
    
    return expressionStatement();
}

std::unique_ptr<Statement> Parser::ifStatement() {
    if (!match({TokenType::LEFT_PAREN})) {
        error("Expected '(' after 'if'");
        return nullptr;
    }
    
    auto condition = expression();
    
    if (!match({TokenType::RIGHT_PAREN})) {
        error("Expected ')' after if condition");
        return nullptr;
    }
    
    auto thenBranch = statement();
    std::unique_ptr<Statement> elseBranch = nullptr;
    
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }
    
    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::whileStatement() {
    if (!match({TokenType::LEFT_PAREN})) {
        error("Expected '(' after 'while'");
        return nullptr;
    }
    
    auto condition = expression();
    
    if (!match({TokenType::RIGHT_PAREN})) {
        error("Expected ')' after while condition");
        return nullptr;
    }
    
    auto body = statement();
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::returnStatement() {
    std::unique_ptr<Expression> value = nullptr;
    
    if (!check(TokenType::SEMICOLON) && !check(TokenType::NEWLINE)) {
        value = expression();
    }
    
    match({TokenType::SEMICOLON, TokenType::NEWLINE});
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Statement> Parser::printStatement() {
    auto expr = expression();
    match({TokenType::SEMICOLON, TokenType::NEWLINE});
    return std::make_unique<PrintStatement>(std::move(expr));
}

std::unique_ptr<BlockStatement> Parser::blockStatement() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match({TokenType::NEWLINE})) continue;
        auto stmt = declaration();
        if (stmt) statements.push_back(std::move(stmt));
    }
    
    if (!match({TokenType::RIGHT_BRACE})) {
        error("Expected '}' after block");
        return nullptr;
    }
    
    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr = expression();
    match({TokenType::SEMICOLON, TokenType::NEWLINE});
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expression> Parser::assignment() {
    auto expr = logicalOr();
    
    if (match({TokenType::ASSIGN})) {
        auto value = assignment();
        
        if (auto identifier = dynamic_cast<IdentifierExpression*>(expr.get())) {
            std::string name = identifier->name;
            expr.release(); // Release ownership since we're creating a new expression
            return std::make_unique<BinaryExpression>(
                std::make_unique<IdentifierExpression>(name), "=", std::move(value));
        }
        
        error("Invalid assignment target");
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match({TokenType::OR})) {
        std::string op = previous().lexeme;
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match({TokenType::AND})) {
        std::string op = previous().lexeme;
        auto right = equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::NOT_EQUAL, TokenType::EQUAL})) {
        std::string op = previous().lexeme;
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = term();
    
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        std::string op = previous().lexeme;
        auto right = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr = factor();
    
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        std::string op = previous().lexeme;
        auto right = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr = unary();
    
    while (match({TokenType::DIVIDE, TokenType::MULTIPLY, TokenType::MODULO})) {
        std::string op = previous().lexeme;
        auto right = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        auto right = unary();
        return std::make_unique<UnaryExpression>(op, std::move(right));
    }
    
    return call();
}

std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();
    
    while (match({TokenType::LEFT_PAREN})) {
        auto args = arguments();
        if (!match({TokenType::RIGHT_PAREN})) {
            error("Expected ')' after arguments");
            return nullptr;
        }
        expr = std::make_unique<CallExpression>(std::move(expr), std::move(args));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match({TokenType::TRUE})) {
        return std::make_unique<LiteralExpression>(true);
    }
    
    if (match({TokenType::FALSE})) {
        return std::make_unique<LiteralExpression>(false);
    }
    
    if (match({TokenType::NIL})) {
        return std::make_unique<LiteralExpression>(nullptr);
    }
    
    if (match({TokenType::NUMBER})) {
        double value = std::stod(previous().lexeme);
        return std::make_unique<LiteralExpression>(value);
    }
    
    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpression>(previous().lexeme);
    }
    
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpression>(previous().lexeme);
    }
    
    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        if (!match({TokenType::RIGHT_PAREN})) {
            error("Expected ')' after expression");
            return nullptr;
        }
        return expr;
    }
    
    error("Expected expression");
    return nullptr;
}

std::vector<std::unique_ptr<Expression>> Parser::arguments() {
    std::vector<std::unique_ptr<Expression>> args;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    
    return args;
}

void Parser::error(const std::string& message) {
    std::string errorMsg = "Parse error at line " + std::to_string(peek().line) + ": " + message;
    throw std::runtime_error(errorMsg);
}