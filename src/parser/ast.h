#pragma once
#include "src/catalog/column.h"
#include <string>
#include <vector>
#include <memory>

namespace minidb {

enum class StatementType {
    CREATE_TABLE,
    INSERT,
    SELECT,
    DELETE,
    CREATE_INDEX
};

struct Statement {
    virtual ~Statement() = default;
    virtual StatementType GetType() const = 0;
};

struct CreateTableStatement : public Statement {
    std::string table_name;
    std::vector<Column> columns;
    StatementType GetType() const override { return StatementType::CREATE_TABLE; }
};

struct InsertStatement : public Statement {
    std::string table_name;
    std::vector<std::string> raw_values;
    StatementType GetType() const override { return StatementType::INSERT; }
};

struct WhereClause {
    std::string column_name;
    std::string value; // raw string value for equality
};

struct SelectStatement : public Statement {
    std::string table_name;
    std::unique_ptr<WhereClause> where;
    StatementType GetType() const override { return StatementType::SELECT; }
};

struct DeleteStatement : public Statement {
    std::string table_name;
    std::unique_ptr<WhereClause> where;
    StatementType GetType() const override { return StatementType::DELETE; }
};

struct CreateIndexStatement : public Statement {
    std::string index_name;
    std::string table_name;
    std::string column_name;
    StatementType GetType() const override { return StatementType::CREATE_INDEX; }
};

} // namespace minidb
