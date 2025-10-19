#pragma once
#include "lexer.h"
#include "ast.h"
#include <memory>
#include <vector>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
    
private:
    std::vector<Token> tokens;
    size_t current;
    
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(std::vector<TokenType> types);
    void synchronize();
    
    // Parsing methods
    std::unique_ptr<Program> program();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<VarDeclaration> varDeclaration();
    std::unique_ptr<FunctionDeclaration> functionDeclaration();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<BlockStatement> blockStatement();
    std::unique_ptr<Statement> expressionStatement();
    
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
    
    std::vector<std::unique_ptr<Expression>> arguments();
    
    void error(const std::string& message);
};