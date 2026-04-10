#pragma once
#include "src/parser/lexer.h"
#include "src/parser/ast.h"
#include "src/common/status.h"
#include <memory>

namespace minidb {

class Parser {
public:
    Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)), pos_(0) {}

    std::unique_ptr<Statement> Parse(Status& status);

private:
    const Token& Peek() const { return tokens_[pos_]; }
    const Token& Consume() { return tokens_[pos_++]; }
    bool Match(TokenType type);
    bool Expect(TokenType type, Status& status, const std::string& msg);

    std::unique_ptr<Statement> ParseCreateTable(Status& status);
    std::unique_ptr<Statement> ParseInsert(Status& status);
    std::unique_ptr<Statement> ParseSelect(Status& status);
    std::unique_ptr<Statement> ParseDelete(Status& status);
    std::unique_ptr<Statement> ParseCreateIndex(Status& status);
    std::unique_ptr<WhereClause> ParseWhere(Status& status);

    std::vector<Token> tokens_;
    size_t pos_;
};

} // namespace minidb
