#pragma once
#include "src/parser/ast.h"
#include "src/catalog/catalog.h"
#include "src/storage/table_heap.h"
#include "src/common/status.h"
#include <memory>
#include <iostream>

namespace minidb {

class Executor {
public:
    Executor(Catalog& catalog, Pager& pager) : catalog_(catalog), pager_(pager) {}

    Status Execute(const Statement& stmt, std::ostream& out = std::cout);

private:
    Status ExecuteCreate(const CreateTableStatement& stmt, std::ostream& out);
    Status ExecuteInsert(const InsertStatement& stmt, std::ostream& out);
    Status ExecuteSelect(const SelectStatement& stmt, std::ostream& out);
    Status ExecuteDelete(const DeleteStatement& stmt, std::ostream& out);
    Status ExecuteCreateIndex(const CreateIndexStatement& stmt, std::ostream& out);

    Catalog& catalog_;
    Pager& pager_;
    // For v1, we assume a single table in a single file for simplicity, 
    // or we'd map table names to files.
    std::unique_ptr<TableHeap> current_table_;
};

} // namespace minidb
