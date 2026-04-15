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
    BTreeIndex(std::string name, std::string table_name, std::vector<std::string> column_names)
        : name_(std::move(name)), table_name_(std::move(table_name)), column_names_(std::move(column_names)) {}

    void Insert(const std::vector<Value>& keys, const Record& record) {
        data_.insert({ValuesToKey(keys), record});
    }

    std::vector<Record> LookupRange(const std::vector<Value>& low, bool include_low, const std::vector<Value>& high, bool include_high) {
        std::vector<Record> results;
        auto it_low = include_low ? data_.lower_bound(ValuesToKey(low)) : data_.upper_bound(ValuesToKey(low));
        auto it_high = include_high ? data_.upper_bound(ValuesToKey(high)) : data_.lower_bound(ValuesToKey(high));

        for (auto it = it_low; it != it_high; ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    // Lookup for equality
    std::vector<Record> Lookup(const std::vector<Value>& keys) {
        std::vector<Record> results;
        auto range = data_.equal_range(ValuesToKey(keys));
        for (auto it = range.first; it != range.second; ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    // Lookup for open-ended range (e.g., > val)
    std::vector<Record> LookupGreater(const std::vector<Value>& vals, bool inclusive) {
        std::vector<Record> results;
        auto it_start = inclusive ? data_.lower_bound(ValuesToKey(vals)) : data_.upper_bound(ValuesToKey(vals));
        for (auto it = it_start; it != data_.end(); ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    // Lookup for open-ended range (e.g., < val)
    std::vector<Record> LookupLess(const std::vector<Value>& vals, bool inclusive) {
        std::vector<Record> results;
        auto it_end = inclusive ? data_.upper_bound(ValuesToKey(vals)) : data_.lower_bound(ValuesToKey(vals));
        for (auto it = data_.begin(); it != it_end; ++it) {
            results.push_back(it->second);
        }
        return results;
    }

    const std::vector<std::string>& GetColumnNames() const { return column_names_; }

private:
    std::string ValuesToKey(const std::vector<Value>& keys) {
        std::string result;
        for (const auto& v : keys) {
            if (v.GetType() == DataType::INT) {
                char buf[12];
                std::sprintf(buf, "%010d", v.AsInt());
                result += std::string(buf);
            } else {
                result += v.AsString();
            }
            result += "|";
        }
        return result;
    }

    std::string name_;
    std::string table_name_;
    std::vector<std::string> column_names_;
    std::multimap<std::string, Record> data_;
};

} // namespace minidb
