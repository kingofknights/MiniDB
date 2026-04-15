#include "src/parser/parser.h"

namespace minidb {

std::unique_ptr<Statement> Parser::Parse(Status& status) {
    if (Match(TokenType::EXPLAIN)) {
        auto explain = std::make_unique<ExplainStatement>();
        explain->stmt = Parse(status);
        return explain;
    }

    std::unique_ptr<Statement> stmt;
    if (Match(TokenType::CREATE)) {
        if (Peek().type == TokenType::INDEX) stmt = ParseCreateIndex(status);
        else stmt = ParseCreateTable(status);
    }
    else if (Match(TokenType::INSERT)) stmt = ParseInsert(status);
    else if (Match(TokenType::SELECT)) stmt = ParseSelect(status);
    else if (Match(TokenType::DELETE)) stmt = ParseDelete(status);
    else if (Match(TokenType::UPDATE)) stmt = ParseUpdate(status);
    else if (Match(TokenType::BEGIN)) return ParseTransaction(TransactionType::BEGIN, status);
    else if (Match(TokenType::COMMIT)) return ParseTransaction(TransactionType::COMMIT, status);
    else if (Match(TokenType::ROLLBACK)) return ParseTransaction(TransactionType::ROLLBACK, status);
    else {
        status = Status::IOError("Unexpected token: " + Peek().text);
        return nullptr;
    }

    if (stmt && status.ok()) {
        Match(TokenType::SEMICOLON);
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
    for (auto & c: stmt->table_name) c = std::toupper(c);

    if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
    bool first = true;
    while (Peek().type != TokenType::RPAREN) {
        if (!first && !Expect(TokenType::COMMA, status, "Expected comma")) return nullptr;
        first = false;

        if (Match(TokenType::FOREIGN)) {
            if (!Expect(TokenType::KEY, status, "Expected KEY after FOREIGN")) return nullptr;
            if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
            if (Peek().type != TokenType::IDENTIFIER) {
                status = Status::IOError("Expected column name in FOREIGN KEY");
                return nullptr;
            }
            std::string col = Consume().text;
            for (auto & c: col) c = std::toupper(c);
            if (!Expect(TokenType::RPAREN, status, "Expected )")) return nullptr;

            if (!Expect(TokenType::REFERENCES, status, "Expected REFERENCES")) return nullptr;
            if (Peek().type != TokenType::IDENTIFIER) {
                status = Status::IOError("Expected referenced table name");
                return nullptr;
            }
            std::string ref_table = Consume().text;
            for (auto & c: ref_table) c = std::toupper(c);

            if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
            if (Peek().type != TokenType::IDENTIFIER) {
                status = Status::IOError("Expected referenced column name");
                return nullptr;
            }
            std::string ref_col = Consume().text;
            for (auto & c: ref_col) c = std::toupper(c);
            if (!Expect(TokenType::RPAREN, status, "Expected )")) return nullptr;

            stmt->foreign_keys.push_back({col, ref_table, ref_col});
            continue;
        }

        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected column name");
            return nullptr;
        }
        std::string col_name = Consume().text;
        for (auto & c: col_name) c = std::toupper(c);

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
    for (auto & c: stmt->index_name) c = std::toupper(c);

    if (!Expect(TokenType::ON, status, "Expected ON")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    for (auto & c: stmt->table_name) c = std::toupper(c);

    if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
    bool first = true;
    while (Peek().type != TokenType::RPAREN) {
        if (!first && !Expect(TokenType::COMMA, status, "Expected comma")) return nullptr;
        first = false;
        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected column name");
            return nullptr;
        }
        std::string col = Consume().text;
        for (auto & c: col) c = std::toupper(c);
        stmt->column_names.push_back(col);
    }
    if (stmt->column_names.empty()) {
        status = Status::IOError("Index must have at least one column");
        return nullptr;
    }
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
    for (auto & c: stmt->table_name) c = std::toupper(c);

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
    if (Match(TokenType::STAR)) {
    } else {
        bool first = true;
        while (true) {
            if (!first && !Expect(TokenType::COMMA, status, "Expected comma between select items")) return nullptr;
            first = false;
            AggregateType agg_type;
            if (Match(TokenType::COUNT)) agg_type = AggregateType::COUNT;
            else if (Match(TokenType::SUM)) agg_type = AggregateType::SUM;
            else if (Match(TokenType::AVG)) agg_type = AggregateType::AVG;
            else if (Match(TokenType::MIN)) agg_type = AggregateType::MIN;
            else if (Match(TokenType::MAX)) agg_type = AggregateType::MAX;
            else {
                status = Status::IOError("Expected aggregate function (COUNT, SUM, etc.) or *");
                return nullptr;
            }
            if (!Expect(TokenType::LPAREN, status, "Expected (")) return nullptr;
            std::string col;
            if (Match(TokenType::STAR)) col = "*";
            else {
                if (Peek().type != TokenType::IDENTIFIER) {
                    status = Status::IOError("Expected column name in aggregate");
                    return nullptr;
                }
                col = Consume().text;
                for (auto & c: col) c = std::toupper(c);
            }
            if (!Expect(TokenType::RPAREN, status, "Expected )")) return nullptr;
            stmt->aggregates.push_back({agg_type, col});
            if (Peek().type != TokenType::COMMA) break;
        }
    }
    if (!Expect(TokenType::FROM, status, "Expected FROM")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    for (auto & c: stmt->table_name) c = std::toupper(c);

    if (Match(TokenType::JOIN)) {
        auto join = std::make_unique<JoinClause>();
        join->left_table = stmt->table_name;
        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected table name in JOIN");
            return nullptr;
        }
        join->right_table = Consume().text;
        for (auto & c: join->right_table) c = std::toupper(c);
        join->type = JoinType::INNER;
        if (!Expect(TokenType::ON, status, "Expected ON in JOIN")) return nullptr;
        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected left column in JOIN ON");
            return nullptr;
        }
        join->left_column = Consume().text;
        for (auto & c: join->left_column) c = std::toupper(c);
        if (!Expect(TokenType::EQUAL, status, "Expected = in JOIN ON")) return nullptr;
        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected right column in JOIN ON");
            return nullptr;
        }
        join->right_column = Consume().text;
        for (auto & c: join->right_column) c = std::toupper(c);
        stmt->join = std::move(join);
    }

    stmt->where = ParseWhere(status);

    if (Match(TokenType::GROUP)) {
        if (!Expect(TokenType::BY, status, "Expected BY after GROUP")) return nullptr;
        if (Peek().type != TokenType::IDENTIFIER) {
            status = Status::IOError("Expected column name in GROUP BY");
            return nullptr;
        }
        stmt->group_by_column = Consume().text;
        for (auto & c: stmt->group_by_column) c = std::toupper(c);
    }
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
    for (auto & c: stmt->table_name) c = std::toupper(c);
    stmt->where = ParseWhere(status);
    return stmt;
}

std::unique_ptr<Statement> Parser::ParseUpdate(Status& status) {
    auto stmt = std::make_unique<UpdateStatement>();
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected table name in UPDATE");
        return nullptr;
    }
    stmt->table_name = Consume().text;
    for (auto & c: stmt->table_name) c = std::toupper(c);
    if (!Expect(TokenType::SET, status, "Expected SET after table name")) return nullptr;
    if (Peek().type != TokenType::IDENTIFIER) {
        status = Status::IOError("Expected column name in SET");
        return nullptr;
    }
    stmt->column_name = Consume().text;
    for (auto & c: stmt->column_name) c = std::toupper(c);
    if (!Expect(TokenType::EQUAL, status, "Expected = in SET")) return nullptr;
    if (Peek().type != TokenType::INTEGER_LITERAL && Peek().type != TokenType::STRING_LITERAL) {
        status = Status::IOError("Expected new value in SET");
        return nullptr;
    }
    stmt->new_value = Consume().text;
    stmt->where = ParseWhere(status);
    return stmt;
}

std::unique_ptr<Statement> Parser::ParseTransaction(TransactionType type, Status& status) {
    auto stmt = std::make_unique<TransactionStatement>();
    stmt->type = type;
    Match(TokenType::SEMICOLON);
    return stmt;
}

} // namespace minidb
