#include "src/parser/lexer.h"
#include <cctype>
#include <unordered_map>

namespace minidb {

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"CREATE", TokenType::CREATE}, {"TABLE", TokenType::TABLE},
    {"INSERT", TokenType::INSERT}, {"INTO", TokenType::INTO}, {"VALUES", TokenType::VALUES},
    {"SELECT", TokenType::SELECT}, {"FROM", TokenType::FROM}, {"DELETE", TokenType::DELETE},
    {"WHERE", TokenType::WHERE}, {"INDEX", TokenType::INDEX}, {"ON", TokenType::ON},
    {"INT", TokenType::INT}, {"TEXT", TokenType::TEXT}
};

std::vector<Token> Lexer::Tokenize() {
    std::vector<Token> tokens;
    Token t;
    do {
        t = NextToken();
        tokens.push_back(t);
    } while (t.type != TokenType::END_OF_FILE);
    return tokens;
}

void Lexer::SkipWhitespace() {
    while (std::isspace(Peek())) Consume();
}

Token Lexer::NextToken() {
    SkipWhitespace();
    char c = Peek();
    if (c == '\0') return {TokenType::END_OF_FILE, ""};

    if (std::isalpha(c) || c == '_') {
        std::string text;
        while (std::isalnum(Peek()) || Peek() == '_') {
            text += std::toupper(Consume());
        }
        if (KEYWORDS.count(text)) return {KEYWORDS.at(text), text};
        return {TokenType::IDENTIFIER, text};
    }

    if (std::isdigit(c)) {
        std::string text;
        while (std::isdigit(Peek())) text += Consume();
        return {TokenType::INTEGER_LITERAL, text};
    }

    if (c == '"' || c == '\'') {
        char quote = Consume();
        std::string text;
        while (Peek() != quote && Peek() != '\0') text += Consume();
        if (Peek() == quote) Consume();
        return {TokenType::STRING_LITERAL, text};
    }

    Consume();
    switch (c) {
        case '(': return {TokenType::LPAREN, "("};
        case ')': return {TokenType::RPAREN, ")"};
        case ',': return {TokenType::COMMA, ","};
        case '*': return {TokenType::STAR, "*"};
        case ';': return {TokenType::SEMICOLON, ";"};
        case '=': return {TokenType::EQUAL, "="};
    }

    return {TokenType::UNKNOWN, std::string(1, c)};
}

} // namespace minidb
