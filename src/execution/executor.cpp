#include "src/execution/executor.h"
#include "src/storage/index.h"
#include "src/storage/btree.h"
#include <iomanip>
#include <cstring>

namespace minidb {

template<typename T>
bool Compare(const T& a, const T& b, OpType op) {
    switch (op) {
        case OpType::EQUAL: return a == b;
        case OpType::GREATER: return a > b;
        case OpType::LESS: return a < b;
        case OpType::GREATER_EQUAL: return a >= b;
        case OpType::LESS_EQUAL: return a <= b;
        default: return false;
    }
}

bool Matches(const Record& record, const Schema& schema, const WhereClause* where) {
    if (!where) return true;
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        if (schema.GetColumn(i).GetName() == where->column_name) {
            const auto& val = record.GetValue(i);
            if (val.GetType() == DataType::INT) {
                return Compare(val.AsInt(), std::stoi(where->value), where->op);
            } else {
                return Compare(val.AsString(), where->value, where->op);
            }
        }
    }
    return false;
}

Status Executor::Execute(const Statement& stmt, std::ostream& out) {
    switch (stmt.GetType()) {
        case StatementType::CREATE_TABLE:
            return ExecuteCreate(static_cast<const CreateTableStatement&>(stmt), out);
        case StatementType::INSERT:
            return ExecuteInsert(static_cast<const InsertStatement&>(stmt), out);
        case StatementType::SELECT:
            return ExecuteSelect(static_cast<const SelectStatement&>(stmt), out);
        case StatementType::DELETE:
            return ExecuteDelete(static_cast<const DeleteStatement&>(stmt), out);
        case StatementType::CREATE_INDEX:
            return ExecuteCreateIndex(static_cast<const CreateIndexStatement&>(stmt), out);
        default:
            return Status::IOError("Execution for this statement type is not yet implemented");
    }
}

Status Executor::ExecuteCreate(const CreateTableStatement& stmt, std::ostream& out) {
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

    out << "Table created: " << stmt.table_name << std::endl;
    return Status::OK();
}

Status Executor::ExecuteInsert(const InsertStatement& stmt, std::ostream& out) {
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

    Record rec(std::move(values));
    Status s = current_table_->InsertRecord(rec);
    if (s.ok()) {
        // Update hash indexes
        auto h_indexes = catalog_.GetHashIndexes(stmt.table_name);
        for (auto* idx : h_indexes) {
            for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
                if (schema.GetColumn(i).GetName() == idx->GetColumnName()) {
                    idx->Insert(rec.GetValue(i), rec);
                    break;
                }
            }
        }
        // Update btree indexes
        auto b_indexes = catalog_.GetBTreeIndexes(stmt.table_name);
        for (auto* idx : b_indexes) {
            for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
                if (schema.GetColumn(i).GetName() == idx->GetColumnName()) {
                    idx->Insert(rec.GetValue(i), rec);
                    break;
                }
            }
        }
        out << "1 row inserted" << std::endl;
    }
    return s;
}

Status Executor::ExecuteSelect(const SelectStatement& stmt, std::ostream& out) {
    if (!catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table not found: " + stmt.table_name);
    }
    const Schema& schema = catalog_.GetSchema(stmt.table_name);
    
    if (!current_table_) {
        current_table_ = std::make_unique<TableHeap>(pager_, schema);
    }

    std::vector<Record> records;
    bool used_index = false;

    if (stmt.where) {
        // Prioritize B-Tree for range support or equality
        auto b_indexes = catalog_.GetBTreeIndexes(stmt.table_name);
        for (auto* idx : b_indexes) {
            if (idx->GetColumnName() == stmt.where->column_name) {
                for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
                    if (schema.GetColumn(i).GetName() == idx->GetColumnName()) {
                        Value key(0);
                        if (schema.GetColumn(i).GetType() == DataType::INT) key = Value(std::stoi(stmt.where->value));
                        else key = Value(stmt.where->value);
                        
                        switch (stmt.where->op) {
                            case OpType::EQUAL: records = idx->Lookup(key); break;
                            case OpType::GREATER: records = idx->LookupGreater(key, false); break;
                            case OpType::GREATER_EQUAL: records = idx->LookupGreater(key, true); break;
                            case OpType::LESS: records = idx->LookupLess(key, false); break;
                            case OpType::LESS_EQUAL: records = idx->LookupLess(key, true); break;
                        }
                        used_index = true;
                        break;
                    }
                }
            }
            if (used_index) break;
        }

        // Fallback to Hash only for equality if no B-Tree available
        if (!used_index && stmt.where->op == OpType::EQUAL) {
            auto h_indexes = catalog_.GetHashIndexes(stmt.table_name);
            for (auto* idx : h_indexes) {
                if (idx->GetColumnName() == stmt.where->column_name) {
                    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
                        if (schema.GetColumn(i).GetName() == idx->GetColumnName()) {
                            Value key(0);
                            if (schema.GetColumn(i).GetType() == DataType::INT) key = Value(std::stoi(stmt.where->value));
                            else key = Value(stmt.where->value);
                            
                            records = idx->Lookup(key);
                            used_index = true;
                            break;
                        }
                    }
                }
                if (used_index) break;
            }
        }
    }

    if (!used_index) {
        auto all_records = current_table_->Scan();
        for (auto& rec : all_records) {
            if (Matches(rec, schema, stmt.where.get())) {
                records.push_back(std::move(rec));
            }
        }
    }

    if (used_index) out << "(Used index lookup)" << std::endl;
    
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        out << std::setw(15) << schema.GetColumn(i).GetName() << (i == schema.GetColumnCount() - 1 ? "" : " | ");
    }
    out << "\n" << std::string(18 * schema.GetColumnCount(), '-') << "\n";

    for (const auto& record : records) {
        for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
            const auto& val = record.GetValue(i);
            if (val.GetType() == DataType::INT) out << std::setw(15) << val.AsInt();
            else out << std::setw(15) << val.AsString();
            out << (i == schema.GetColumnCount() - 1 ? "" : " | ");
        }
        out << "\n";
    }
    
    out << "(" << records.size() << " rows)" << std::endl;
    return Status::OK();
}

Status Executor::ExecuteDelete(const DeleteStatement& stmt, std::ostream& out) {
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
            Status s = current_table_->UpdatePage(i, page);
            if (!s.ok()) return s;
        }
    }

    out << deleted_count << " rows deleted" << std::endl;
    return Status::OK();
}

Status Executor::ExecuteCreateIndex(const CreateIndexStatement& stmt, std::ostream& out) {
    if (!catalog_.TableExists(stmt.table_name)) {
        return Status::IOError("Table not found: " + stmt.table_name);
    }
    const Schema& schema = catalog_.GetSchema(stmt.table_name);
    
    bool col_found = false;
    size_t col_idx = 0;
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        if (schema.GetColumn(i).GetName() == stmt.column_name) {
            col_found = true;
            col_idx = i;
            break;
        }
    }
    if (!col_found) return Status::IOError("Column not found: " + stmt.column_name);

    IndexType type = (stmt.index_name.find("HASH") != std::string::npos) ? IndexType::HASH : IndexType::BTREE;
    catalog_.AddIndex(stmt.index_name, stmt.table_name, stmt.column_name, type);
    
    if (!current_table_) {
        current_table_ = std::make_unique<TableHeap>(pager_, schema);
    }
    auto all_records = current_table_->Scan();

    if (type == IndexType::HASH) {
        auto idxs = catalog_.GetHashIndexes(stmt.table_name);
        auto* idx = idxs.back();
        for (const auto& rec : all_records) idx->Insert(rec.GetValue(col_idx), rec);
    } else {
        auto idxs = catalog_.GetBTreeIndexes(stmt.table_name);
        auto* idx = idxs.back();
        for (const auto& rec : all_records) idx->Insert(rec.GetValue(col_idx), rec);
    }

    out << "Index created: " << stmt.index_name << std::endl;
    return Status::OK();
}

} // namespace minidb
