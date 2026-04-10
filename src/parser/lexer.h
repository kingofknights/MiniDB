#pragma once
#include <string>
#include <vector>

namespace minidb {

enum class TokenType {
    // Keywords
    CREATE, TABLE, INSERT, INTO, VALUES, SELECT, FROM, DELETE,
    WHERE,
    INT, TEXT,
    
    // Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    STRING_LITERAL,
    
    // Punctuation
    LPAREN, RPAREN, COMMA, STAR, SEMICOLON, EQUAL,
    
    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
};

class Lexer {
public:
    Lexer(std::string input) : input_(std::move(input)), pos_(0) {}

    std::vector<Token> Tokenize();

private:
    char Peek() const { return pos_ < input_.length() ? input_[pos_] : '\0'; }
    char Consume() { return input_[pos_++]; }
    void SkipWhitespace();
    Token NextToken();

    std::string input_;
    size_t pos_;
};

} // namespace minidb
