#include "lexer.h"
#include <cctype>
#include <iostream>

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::LET},
    {"fun", TokenType::FUN},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"nil", TokenType::NIL},
    {"return", TokenType::RETURN},
    {"print", TokenType::PRINT},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT}
};

Lexer::Lexer(const std::string& source) 
    : source(source), current(0), line(1), column(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        char c = peek();
        
        // Skip comments
        if (c == '/' && peekNext() == '/') {
            skipComment();
            continue;
        }
        
        Token token(TokenType::INVALID, "", line, column);
        
        switch (c) {
            case '(': token = makeToken(TokenType::LEFT_PAREN, "("); advance(); break;
            case ')': token = makeToken(TokenType::RIGHT_PAREN, ")"); advance(); break;
            case '{': token = makeToken(TokenType::LEFT_BRACE, "{"); advance(); break;
            case '}': token = makeToken(TokenType::RIGHT_BRACE, "}"); advance(); break;
            case ',': token = makeToken(TokenType::COMMA, ","); advance(); break;
            case ';': token = makeToken(TokenType::SEMICOLON, ";"); advance(); break;
            case '+': token = makeToken(TokenType::PLUS, "+"); advance(); break;
            case '-': token = makeToken(TokenType::MINUS, "-"); advance(); break;
            case '*': token = makeToken(TokenType::MULTIPLY, "*"); advance(); break;
            case '/': token = makeToken(TokenType::DIVIDE, "/"); advance(); break;
            case '%': token = makeToken(TokenType::MODULO, "%"); advance(); break;
            case '\n': token = makeToken(TokenType::NEWLINE, "\\n"); advance(); line++; column = 1; break;
            
            case '=':
                advance();
                if (peek() == '=') {
                    token = makeToken(TokenType::EQUAL, "==");
                    advance();
                } else {
                    token = makeToken(TokenType::ASSIGN, "=");
                }
                break;
                
            case '!':
                advance();
                if (peek() == '=') {
                    token = makeToken(TokenType::NOT_EQUAL, "!=");
                    advance();
                } else {
                    token = makeToken(TokenType::NOT, "!");
                }
                break;
                
            case '<':
                advance();
                if (peek() == '=') {
                    token = makeToken(TokenType::LESS_EQUAL, "<=");
                    advance();
                } else {
                    token = makeToken(TokenType::LESS, "<");
                }
                break;
                
            case '>':
                advance();
                if (peek() == '=') {
                    token = makeToken(TokenType::GREATER_EQUAL, ">=");
                    advance();
                } else {
                    token = makeToken(TokenType::GREATER, ">");
                }
                break;
                
            case '"':
                token = makeString();
                break;
                
            default:
                if (std::isdigit(c)) {
                    token = makeNumber();
                } else if (std::isalpha(c) || c == '_') {
                    token = makeIdentifier();
                } else {
                    std::cerr << "Unexpected character: " << c << " at line " << line << std::endl;
                    advance();
                    continue;
                }
                break;
        }
        
        if (token.type != TokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    column++;
    return source[current++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

Token Lexer::makeNumber() {
    int startColumn = column;
    std::string number;
    
    while (!isAtEnd() && (std::isdigit(peek()) || peek() == '.')) {
        number += advance();
    }
    
    return Token(TokenType::NUMBER, number, line, startColumn);
}

Token Lexer::makeString() {
    int startColumn = column;
    advance(); // Skip opening quote
    
    std::string value;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        value += advance();
    }
    
    if (isAtEnd()) {
        std::cerr << "Unterminated string at line " << line << std::endl;
        return Token(TokenType::INVALID, value, line, startColumn);
    }
    
    advance(); // Skip closing quote
    return Token(TokenType::STRING, value, line, startColumn);
}

Token Lexer::makeIdentifier() {
    int startColumn = column;
    std::string identifier;
    
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        identifier += advance();
    }
    
    // Check if it's a keyword
    auto it = keywords.find(identifier);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    
    return Token(type, identifier, line, startColumn);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, line, column);
}