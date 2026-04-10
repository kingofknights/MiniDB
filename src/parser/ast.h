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
    CREATE_INDEX,
    UPDATE
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

enum class OpType {
    EQUAL,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL
};

struct WhereClause {
    std::string column_name;
    OpType op;
    std::string value; // raw string value
};

enum class JoinType {
    INNER
};

struct JoinClause {
    std::string left_table;
    std::string right_table;
    std::string left_column;
    std::string right_column;
    JoinType type;
};

struct SelectStatement : public Statement {
    std::string table_name;
    std::unique_ptr<WhereClause> where;
    std::unique_ptr<JoinClause> join;
    StatementType GetType() const override { return StatementType::SELECT; }
};

struct DeleteStatement : public Statement {
    std::string table_name;
    std::unique_ptr<WhereClause> where;
    StatementType GetType() const override { return StatementType::DELETE; }
};

struct UpdateStatement : public Statement {
    std::string table_name;
    std::string column_name;
    std::string new_value;
    std::unique_ptr<WhereClause> where;
    StatementType GetType() const override { return StatementType::UPDATE; }
};

struct CreateIndexStatement : public Statement {
    std::string index_name;
    std::string table_name;
    std::string column_name;
    StatementType GetType() const override { return StatementType::CREATE_INDEX; }
};

} // namespace minidb
