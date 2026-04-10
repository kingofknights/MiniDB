#include "src/execution/executor.h"
#include <iomanip>
#include <cstring>

namespace minidb {

bool Matches(const Record& record, const Schema& schema, const WhereClause* where) {
    if (!where) return true;
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        if (schema.GetColumn(i).GetName() == where->column_name) {
            const auto& val = record.GetValue(i);
            if (val.GetType() == DataType::INT) {
                return std::to_string(val.AsInt()) == where->value;
            } else {
                return val.AsString() == where->value;
            }
        }
    }
    return false;
}

Status Executor::Execute(const Statement& stmt) {
    switch (stmt.GetType()) {
        case StatementType::CREATE_TABLE:
            return ExecuteCreate(static_cast<const CreateTableStatement&>(stmt));
        case StatementType::INSERT:
            return ExecuteInsert(static_cast<const InsertStatement&>(stmt));
        case StatementType::SELECT:
            return ExecuteSelect(static_cast<const SelectStatement&>(stmt));
        case StatementType::DELETE:
            return ExecuteDelete(static_cast<const DeleteStatement&>(stmt));
        default:
            return Status::IOError("Execution for this statement type is not yet implemented");
    }
}

Status Executor::ExecuteCreate(const CreateTableStatement& stmt) {
    if (catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table already exists: " + stmt.table_name);
    }
    catalog_.CreateTable(stmt.table_name, Schema(stmt.columns));
    
    // Save Catalog to Page 0
    std::vector<uint8_t> buffer;
    catalog_.Serialize(buffer);
    if (buffer.size() > PAGE_SIZE) {
        return Status::IOError("Catalog size exceeds PAGE_SIZE (4KB)");
    }
    
    Page page0;
    page0.SetPageID(0);
    std::memcpy(page0.GetData(), buffer.data(), buffer.size());
    Status s = pager_.WritePage(page0);
    if (!s.ok()) return s;

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

    auto all_records = current_table_->Scan();
    std::vector<Record> records;
    for (auto& rec : all_records) {
        if (Matches(rec, schema, stmt.where.get())) {
            records.push_back(std::move(rec));
        }
    }
    
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

Status Executor::ExecuteDelete(const DeleteStatement& stmt) {
    if (!catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table not found: " + stmt.table_name);
    }
    const Schema& schema = catalog_.GetSchema(stmt.table_name);
    
    if (!current_table_) {
        current_table_ = std::make_unique<TableHeap>(pager_, schema);
    }

    uint32_t deleted_count = 0;
    Page page;
    for (PageID i = 1; i < pager_.GetPageCount(); ++i) {
        pager_.ReadPage(i, page);
        struct PageHeader { uint32_t num_records; };
        PageHeader* header = reinterpret_cast<PageHeader*>(page.GetData());
        uint8_t* ptr = page.GetData() + sizeof(PageHeader);
        bool page_modified = false;

        for (uint32_t r = 0; r < header->num_records; ++r) {
            size_t bytes_read = 0;
            Record rec = Record::Deserialize(schema, ptr, bytes_read);
            if (!rec.IsDeleted() && Matches(rec, schema, stmt.where.get())) {
                *ptr = 1; // Mark tombstone
                page_modified = true;
                deleted_count++;
            }
            ptr += bytes_read;
        }

        if (page_modified) {
            Status s = pager_.WritePage(page);
            if (!s.ok()) return s;
        }
    }

    std::cout << deleted_count << " rows deleted" << std::endl;
    return Status::OK();
}
} // namespace minidb
