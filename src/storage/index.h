#pragma once
#include "src/storage/record.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace minidb {

/**
 * A simple in-memory Hash Index for equality lookups.
 * Stores a mapping from a serialized value to a list of matching records.
 * For v1, we store copies of records or pointers to them.
 */
class HashIndex {
public:
    HashIndex(std::string name, std::string table_name, std::string column_name)
        : name_(std::move(name)), table_name_(std::move(table_name)), column_name_(std::move(column_name)) {}

    void Insert(const Value& key, const Record& record) {
        std::string key_str = ValueToString(key);
        index_[key_str].push_back(record);
    }

    std::vector<Record> Lookup(const Value& key) {
        std::string key_str = ValueToString(key);
        if (index_.count(key_str)) {
            return index_[key_str];
        }
        return {};
    }

    void Clear() { index_.clear(); }

    const std::string& GetColumnName() const { return column_name_; }

private:
    std::string ValueToString(const Value& v) {
        if (v.GetType() == DataType::INT) return std::to_string(v.AsInt());
        return v.AsString();
    }

    std::string name_;
    std::string table_name_;
    std::string column_name_;
    std::unordered_map<std::string, std::vector<Record>> index_;
};

} // namespace minidb
