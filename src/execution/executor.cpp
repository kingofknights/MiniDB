#include "src/execution/executor.h"
#include <iomanip>

namespace minidb {

Status Executor::Execute(const Statement& stmt) {
    switch (stmt.GetType()) {
        case StatementType::CREATE_TABLE:
            return ExecuteCreate(static_cast<const CreateTableStatement&>(stmt));
        case StatementType::INSERT:
            return ExecuteInsert(static_cast<const InsertStatement&>(stmt));
        case StatementType::SELECT:
            return ExecuteSelect(static_cast<const SelectStatement&>(stmt));
        default:
            return Status::IOError("Execution for this statement type is not yet implemented");
    }
}

Status Executor::ExecuteCreate(const CreateTableStatement& stmt) {
    if (catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table already exists: " + stmt.table_name);
    }
    catalog_.CreateTable(stmt.table_name, Schema(stmt.columns));
    std::cout << "Table created: " << stmt.table_name << std::endl;
    return Status::OK();
}

Status Executor::ExecuteInsert(const InsertStatement& stmt) {
    if (!catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table not found: " + stmt.table_name);
    }
    const Schema& schema = catalog_.GetSchema(stmt.table_name);
    
    if (stmt.raw_values.size() != schema.GetColumnCount()) {
        return Status::IOError("Column count mismatch");
    }

    std::vector<Value> values;
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        if (schema.GetColumn(i).GetType() == DataType::INT) {
            values.emplace_back(std::stoi(stmt.raw_values[i]));
        } else {
            values.emplace_back(stmt.raw_values[i]);
        }
    }

    if (!current_table_) {
        current_table_ = std::make_unique<TableHeap>(pager_, schema);
    }

    Status s = current_table_->InsertRecord(Record(std::move(values)));
    if (s.ok()) {
        std::cout << "1 row inserted" << std::endl;
    }
    return s;
}

Status Executor::ExecuteSelect(const SelectStatement& stmt) {
    if (!catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table not found: " + stmt.table_name);
    }
    const Schema& schema = catalog_.GetSchema(stmt.table_name);
    
    if (!current_table_) {
        current_table_ = std::make_unique<TableHeap>(pager_, schema);
    }

    auto records = current_table_->Scan();
    
    // Print results simple format
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        std::cout << std::setw(15) << schema.GetColumn(i).GetName() << (i == schema.GetColumnCount() - 1 ? "" : " | ");
    }
    std::cout << "\n" << std::string(18 * schema.GetColumnCount(), '-') << "\n";

    for (const auto& record : records) {
        for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
            const auto& val = record.GetValue(i);
            if (val.GetType() == DataType::INT) {
                std::cout << std::setw(15) << val.AsInt();
            } else {
                std::cout << std::setw(15) << val.AsString();
            }
            std::cout << (i == schema.GetColumnCount() - 1 ? "" : " | ");
        }
        std::cout << "\n";
    }
    
    std::cout << "(" << records.size() << " rows)" << std::endl;
    return Status::OK();
}

} // namespace minidb
