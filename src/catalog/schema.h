#pragma once
#include "src/catalog/column.h"
#include "src/parser/ast.h"
#include <vector>
#include <string>

namespace minidb {

class Schema {
public:
    Schema(std::vector<Column> columns, std::vector<ForeignKey> fks = {}) 
        : columns_(std::move(columns)), foreign_keys_(std::move(fks)) {}

    const std::vector<Column>& GetColumns() const { return columns_; }
    size_t GetColumnCount() const { return columns_.size(); }
    const Column& GetColumn(size_t index) const { return columns_[index]; }
    const std::vector<ForeignKey>& GetForeignKeys() const { return foreign_keys_; }

    void Serialize(std::vector<uint8_t>& buffer) const;
    static Schema Deserialize(const uint8_t* buffer, size_t& offset);

private:
    std::vector<Column> columns_;
    std::vector<ForeignKey> foreign_keys_;
};

} // namespace minidb
