#pragma once
#include "src/catalog/column.h"
#include <vector>
#include <string>

namespace minidb {

class Schema {
public:
    Schema(std::vector<Column> columns) : columns_(std::move(columns)) {}

    const std::vector<Column>& GetColumns() const { return columns_; }
    size_t GetColumnCount() const { return columns_.size(); }
    const Column& GetColumn(size_t index) const { return columns_[index]; }

    void Serialize(std::vector<uint8_t>& buffer) const;
    static Schema Deserialize(const uint8_t* buffer, size_t& offset);

private:
    std::vector<Column> columns_;
};

} // namespace minidb
