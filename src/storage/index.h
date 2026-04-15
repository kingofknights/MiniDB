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
    HashIndex(std::string name, std::string table_name, std::vector<std::string> column_names)
        : name_(std::move(name)), table_name_(std::move(table_name)), column_names_(std::move(column_names)) {}

    void Insert(const std::vector<Value>& keys, const Record& record) {
        std::string key_str = ValuesToString(keys);
        index_[key_str].push_back(record);
    }

    std::vector<Record> Lookup(const std::vector<Value>& keys) {
        std::string key_str = ValuesToString(keys);
        if (index_.count(key_str)) {
            return index_[key_str];
        }
        return {};
    }

    void Clear() { index_.clear(); }

    const std::vector<std::string>& GetColumnNames() const { return column_names_; }

private:
    std::string ValuesToString(const std::vector<Value>& keys) {
        std::string result;
        for (const auto& v : keys) {
            if (v.GetType() == DataType::INT) result += std::to_string(v.AsInt());
            else result += v.AsString();
            result += "|"; // delimiter
        }
        return result;
    }

    std::string name_;
    std::string table_name_;
    std::vector<std::string> column_names_;
    std::unordered_map<std::string, std::vector<Record>> index_;
};

} // namespace minidb
