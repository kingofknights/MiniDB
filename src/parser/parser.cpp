#include "src/parser/parser.h"

namespace minidb {

std::unique_ptr<Statement> Parser::Parse(Status& status) {
    std::unique_ptr<Statement> stmt;
    if (Match(TokenType::CREATE)) {
        if (Peek().type == TokenType::INDEX) stmt = ParseCreateIndex(status);
        else stmt = ParseCreateTable(status);
    }
    else if (Match(TokenType::INSERT)) stmt = ParseInsert(status);
    else if (Match(TokenType::SELECT)) stmt = ParseSelect(status);
    else if (Match(TokenType::DELETE)) stmt = ParseDelete(status);
    else {
        status = Status::IOError("Unexpected token: " + Peek().text);
        return nullptr;
    }

    if (stmt && status.ok()) {
        // Optional semicolon
        Match(TokenType::SEMICOLON);
        // Ensure no more tokens
        if (Peek().type != TokenType::END_OF_FILE) {
            status = Status::IOError("Extra tokens after statement: " + Peek().text);
            return nullptr;
        }
    }

    return stmt;
}

bool Parser::Match(TokenType type) {
    if (Peek().type == type) {
        Consume();
        return true;
    }
    return false;
}

bool Parser::Expect(TokenType type, Status& status, const std::string& msg) {
    if (Peek().type != type) {
        status = Status::IOError(msg + " (found " + Peek().text + ")");
        return false;
    }
    Consume();
    return true;
}

std::unique_ptr<Statement> Parser::ParseCreateTable(Status& status) {
    auto stmt = std::make_unique<CreateTableStatement>();
    if (!Expect(TokenType::TABLE, status, "Expected TABLE")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    
    if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
    
    bool first = true;
    while (Peek().type != TokenType::RPAREN) {
        if (!first && !Expect(TokenType::COMMA, status, "Expected comma")) return nullptr;
        first = false;

        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected column name");
            return nullptr;
        }
        std::string col_name = Consume().text;
        
        DataType type;
        if (Match(TokenType::INT)) type = DataType::INT;
        else if (Match(TokenType::TEXT)) type = DataType::TEXT;
        else {
            status = Status::IOError("Expected INT or TEXT");
            return nullptr;
        }
        
        stmt->columns.emplace_back(col_name, type);
    }
    
    if (stmt->columns.empty()) {
        status = Status::IOError("Table must have at least one column");
        return nullptr;
    }

    if (!Expect(TokenType::RPAREN, status, "Expected )")) return nullptr;
    return stmt;
}

std::unique_ptr<Statement> Parser::ParseCreateIndex(Status& status) {
    auto stmt = std::make_unique<CreateIndexStatement>();
    if (!Expect(TokenType::INDEX, status, "Expected INDEX")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected index name");
        return nullptr;
    }
    stmt->index_name = Consume().text;
    
    if (!Expect(TokenType::ON, status, "Expected ON")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;

    if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected column name");
        return nullptr;
    }
    stmt->column_name = Consume().text;
    if (!Expect(TokenType::RPAREN, status, "Expected )")) return nullptr;

    return stmt;
}

std::unique_ptr<Statement> Parser::ParseInsert(Status& status) {
    auto stmt = std::make_unique<InsertStatement>();
    if (!Expect(TokenType::INTO, status, "Expected INTO")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    
    if (!Expect(TokenType::VALUES, status, "Expected VALUES")) return nullptr;
    if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
    
    bool first = true;
    while (Peek().type != TokenType::RPAREN) {
        if (!first && !Expect(TokenType::COMMA, status, "Expected comma")) return nullptr;
        first = false;

        if (Peek().type != TokenType::INTEGER_LITERAL && Peek().type != TokenType::STRING_LITERAL) {
            status = Status::IOError("Expected value");
            return nullptr;
        }
        stmt->raw_values.push_back(Consume().text);
    }
    
    if (!Expect(TokenType::RPAREN, status, "Expected )")) return nullptr;
    return stmt;
}

std::unique_ptr<WhereClause> Parser::ParseWhere(Status& status) {
    if (!Match(TokenType::WHERE)) return nullptr;
    
    auto where = std::make_unique<WhereClause>();
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected column name in WHERE");
        return nullptr;
    }
    where->column_name = Consume().text;
    for (auto & c: where->column_name) c = std::toupper(c);

    if (Match(TokenType::EQUAL)) where->op = OpType::EQUAL;
    else if (Match(TokenType::GREATER)) where->op = OpType::GREATER;
    else if (Match(TokenType::LESS)) where->op = OpType::LESS;
    else if (Match(TokenType::GREATER_EQUAL)) where->op = OpType::GREATER_EQUAL;
    else if (Match(TokenType::LESS_EQUAL)) where->op = OpType::LESS_EQUAL;
    else {
        status = Status::IOError("Expected operator (=, >, <, >=, <=) in WHERE");
        return nullptr;
    }

    if (Peek().type != TokenType::INTEGER_LITERAL && Peek().type != TokenType::STRING_LITERAL) {
        status = Status::IOError("Expected value in WHERE");
        return nullptr;
    }
    where->value = Consume().text;
    return where;
}

std::unique_ptr<Statement> Parser::ParseSelect(Status& status) {
    auto stmt = std::make_unique<SelectStatement>();
    if (!Expect(TokenType::STAR, status, "Expected *")) return nullptr;
    if (!Expect(TokenType::FROM, status, "Expected FROM")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    stmt->where = ParseWhere(status);
    return stmt;
}

std::unique_ptr<Statement> Parser::ParseDelete(Status& status) {
    auto stmt = std::make_unique<DeleteStatement>();
    if (!Expect(TokenType::FROM, status, "Expected FROM")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    stmt->where = ParseWhere(status);
    return stmt;
}

} // namespace minidb
