#pragma once
#include "src/storage/pager.h"
#include "src/storage/record.h"
#include "src/catalog/schema.h"
#include <vector>
#include <memory>

namespace minidb {

/**
 * TableHeap manages physical storage of rows across pages.
 * For v1, it uses a simple append-only strategy with sequential scans.
 */
class TableHeap {
public:
    TableHeap(Pager& pager, const Schema& schema) : pager_(pager), schema_(schema) {}

    // Insert a record into the table
    Status InsertRecord(const Record& record);

    // Scan all records in the table
    std::vector<Record> Scan();

private:
    Pager& pager_;
    const Schema& schema_;
};

} // namespace minidb
