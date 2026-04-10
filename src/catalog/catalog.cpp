#include "src/catalog/catalog.h"
#include <cstring>

namespace minidb {

void Column::Serialize(std::vector<uint8_t>& buffer) const {
    uint8_t type = static_cast<uint8_t>(type_);
    buffer.push_back(type);
    
    uint32_t name_len = static_cast<uint32_t>(name_.length());
    uint8_t name_len_bytes[4];
    std::memcpy(name_len_bytes, &name_len, 4);
    buffer.insert(buffer.end(), name_len_bytes, name_len_bytes + 4);
    buffer.insert(buffer.end(), name_.begin(), name_.end());
}

Column Column::Deserialize(const uint8_t* buffer, size_t& offset) {
    DataType type = static_cast<DataType>(buffer[offset++]);
    
    uint32_t name_len;
    std::memcpy(&name_len, buffer + offset, 4);
    offset += 4;
    
    std::string name(reinterpret_cast<const char*>(buffer + offset), name_len);
    offset += name_len;
    
    return Column(std::move(name), type);
}

void Schema::Serialize(std::vector<uint8_t>& buffer) const {
    uint32_t col_count = static_cast<uint32_t>(columns_.size());
    uint8_t col_count_bytes[4];
    std::memcpy(col_count_bytes, &col_count, 4);
    buffer.insert(buffer.end(), col_count_bytes, col_count_bytes + 4);
    
    for (const auto& col : columns_) {
        col.Serialize(buffer);
    }
}

Schema Schema::Deserialize(const uint8_t* buffer, size_t& offset) {
    uint32_t col_count;
    std::memcpy(&col_count, buffer + offset, 4);
    offset += 4;
    
    std::vector<Column> columns;
    for (uint32_t i = 0; i < col_count; ++i) {
        columns.push_back(Column::Deserialize(buffer, offset));
    }
    return Schema(std::move(columns));
}

void Catalog::Serialize(std::vector<uint8_t>& buffer) const {
    uint32_t table_count = static_cast<uint32_t>(tables_.size());
    uint8_t table_count_bytes[4];
    std::memcpy(table_count_bytes, &table_count, 4);
    buffer.insert(buffer.end(), table_count_bytes, table_count_bytes + 4);
    
    for (const auto& [name, schema] : tables_) {
        uint32_t name_len = static_cast<uint32_t>(name.length());
        uint8_t name_len_bytes[4];
        std::memcpy(name_len_bytes, &name_len, 4);
        buffer.insert(buffer.end(), name_len_bytes, name_len_bytes + 4);
        buffer.insert(buffer.end(), name.begin(), name.end());
        schema->Serialize(buffer);
    }
}

std::unique_ptr<Catalog> Catalog::Deserialize(const uint8_t* buffer) {
    auto catalog = std::make_unique<Catalog>();
    uint32_t table_count;
    std::memcpy(&table_count, buffer, 4);
    size_t offset = 4;
    
    for (uint32_t i = 0; i < table_count; ++i) {
        uint32_t name_len;
        std::memcpy(&name_len, buffer + offset, 4);
        offset += 4;
        
        std::string table_name(reinterpret_cast<const char*>(buffer + offset), name_len);
        offset += name_len;
        
        Schema schema = Schema::Deserialize(buffer, offset);
        catalog->CreateTable(std::move(table_name), std::move(schema));
    }
    return catalog;
}

} // namespace minidb
