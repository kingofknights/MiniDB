#include "src/catalog/catalog.h"
#include "src/storage/index.h"
#include "src/storage/btree.h"
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

    // Serialize Foreign Keys
    uint32_t fk_count = static_cast<uint32_t>(foreign_keys_.size());
    uint8_t fk_count_bytes[4];
    std::memcpy(fk_count_bytes, &fk_count, 4);
    buffer.insert(buffer.end(), fk_count_bytes, fk_count_bytes + 4);

    for (const auto& fk : foreign_keys_) {
        for (const auto& s : {fk.column_name, fk.referenced_table, fk.referenced_column}) {
            uint32_t len = s.length();
            uint8_t len_bytes[4];
            std::memcpy(len_bytes, &len, 4);
            buffer.insert(buffer.end(), len_bytes, len_bytes + 4);
            buffer.insert(buffer.end(), s.begin(), s.end());
        }
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

    // Deserialize Foreign Keys
    uint32_t fk_count;
    std::memcpy(&fk_count, buffer + offset, 4);
    offset += 4;

    std::vector<ForeignKey> fks;
    for (uint32_t i = 0; i < fk_count; ++i) {
        std::string fields[3];
        for (int j = 0; j < 3; ++j) {
            uint32_t len;
            std::memcpy(&len, buffer + offset, 4);
            offset += 4;
            fields[j] = std::string(reinterpret_cast<const char*>(buffer + offset), len);
            offset += len;
        }
        fks.push_back({fields[0], fields[1], fields[2]});
    }

    return Schema(std::move(columns), std::move(fks));
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

void Catalog::AddIndex(std::string name, std::string table_name, std::vector<std::string> column_names, IndexType type) {
    for (auto & c: name) c = std::toupper(c);
    for (auto & c: table_name) c = std::toupper(c);
    for (auto & col : column_names) {
        for (auto & c : col) c = std::toupper(c);
    }
    
    if (type == IndexType::HASH) {
        hash_indexes_[table_name].push_back(std::make_unique<HashIndex>(name, table_name, column_names));
    } else {
        btree_indexes_[table_name].push_back(std::make_unique<BTreeIndex>(name, table_name, column_names));
    }
}

std::vector<HashIndex*> Catalog::GetHashIndexes(std::string table_name) {
    for (auto & c: table_name) c = std::toupper(c);
    std::vector<HashIndex*> result;
    if (hash_indexes_.count(table_name)) {
        for (const auto& idx : hash_indexes_.at(table_name)) {
            result.push_back(idx.get());
        }
    }
    return result;
}

std::vector<BTreeIndex*> Catalog::GetBTreeIndexes(std::string table_name) {
    for (auto & c: table_name) c = std::toupper(c);
    std::vector<BTreeIndex*> result;
    if (btree_indexes_.count(table_name)) {
        for (const auto& idx : btree_indexes_.at(table_name)) {
            result.push_back(idx.get());
        }
    }
    return result;
}

} // namespace minidb
