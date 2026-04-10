#pragma once
#include "src/storage/record.h"
#include <vector>
#include <map>
#include <memory>

namespace minidb {

/**
 * A simple in-memory B-Tree index for range queries.
 * For v1, we use a map-based or simple vector-based B-Tree simulation.
 * Real disk-based B-Trees would manage PageIDs.
 */
class BTreeIndex {
public:
    BTreeIndex(std::string name, std::string table_name, std::string column_name)
        : name_(std::move(name)), table_name_(std::move(table_name)), column_name_(std::move(column_name)) {}

    void Insert(const Value& key, const Record& record) {
        data_.insert({ValueToKey(key), record});
    }

    std::vector<Record> LookupRange(const Value& low, bool include_low, const Value& high, bool include_high) {
        std::vector<Record> results;
        auto it_low = include_low ? data_.lower_bound(ValueToKey(low)) : data_.upper_bound(ValueToKey(low));
        auto it_high = include_high ? data_.upper_bound(ValueToKey(high)) : data_.lower_bound(ValueToKey(high));

        for (auto it = it_low; it != it_high; ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    // Lookup for equality
    std::vector<Record> Lookup(const Value& key) {
        std::vector<Record> results;
        auto range = data_.equal_range(ValueToKey(key));
        for (auto it = range.first; it != range.second; ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    // Lookup for open-ended range (e.g., > val)
    std::vector<Record> LookupGreater(const Value& val, bool inclusive) {
        std::vector<Record> results;
        auto it_start = inclusive ? data_.lower_bound(ValueToKey(val)) : data_.upper_bound(ValueToKey(val));
        for (auto it = it_start; it != data_.end(); ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    // Lookup for open-ended range (e.g., < val)
    std::vector<Record> LookupLess(const Value& val, bool inclusive) {
        std::vector<Record> results;
        auto it_end = inclusive ? data_.upper_bound(ValueToKey(val)) : data_.lower_bound(ValueToKey(val));
        for (auto it = data_.begin(); it != it_end; ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    const std::string& GetColumnName() const { return column_name_; }

private:
    // To handle both INT and TEXT in a single map, we convert them to a sortable string or a variant.
    // Since our Value is already a variant, we can use a custom comparator or a normalized key.
    // For simplicity, we use a string representation that preserves order for positive ints.
    std::string ValueToKey(const Value& v) {
        if (v.GetType() == DataType::INT) {
            // Padding for string-based sorting of integers
            char buf[12];
            std::sprintf(buf, "%010d", v.AsInt());
            return std::string(buf);
        }
        return v.AsString();
    }

    std::string name_;
    std::string table_name_;
    std::string column_name_;
    std::multimap<std::string, Record> data_;
};

} // namespace minidb
