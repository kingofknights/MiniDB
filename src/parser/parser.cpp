#include "src/parser/parser.h"

namespace minidb {

std::unique_ptr<Statement> Parser::Parse(Status& status) {
    if (Match(TokenType::CREATE)) {
        if (Peek().type == TokenType::INDEX) return ParseCreateIndex(status);
        return ParseCreateTable(status);
    }
    if (Match(TokenType::INSERT)) return ParseInsert(status);
    if (Match(TokenType::SELECT)) return ParseSelect(status);
    if (Match(TokenType::DELETE)) return ParseDelete(status);
    
    status = Status::IOError("Unexpected token: " + Peek().text);
    return nullptr;
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
    
    while (Peek().type != TokenType::RPAREN) {
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
        if (!Match(TokenType::COMMA)) break;
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
    
    while (Peek().type != TokenType::RPAREN) {
        if (Peek().type != TokenType::INTEGER_LITERAL && Peek().type != TokenType::STRING_LITERAL) {
            status = Status::IOError("Expected value");
            return nullptr;
        }
        stmt->raw_values.push_back(Consume().text);
        if (!Match(TokenType::COMMA)) break;
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

    if (!Expect(TokenType::EQUAL, status, "Expected = in WHERE")) return nullptr;

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
