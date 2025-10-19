#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
    // Literals
    NUMBER,
    STRING,
    IDENTIFIER,
    
    // Keywords
    LET,
    FUN,
    IF,
    ELSE,
    WHILE,
    FOR,
    TRUE,
    FALSE,
    NIL,
    RETURN,
    PRINT,
    
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,
    
    // Delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    SEMICOLON,
    
    // Special
    NEWLINE,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t, const std::string& l, int ln, int col) 
        : type(t), lexeme(l), line(ln), column(col) {}
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t current;
    int line;
    int column;
    
    static std::unordered_map<std::string, TokenType> keywords;
    
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    void skipWhitespace();
    void skipComment();
    Token makeNumber();
    Token makeString();
    Token makeIdentifier();
    Token makeToken(TokenType type, const std::string& lexeme = "");
};