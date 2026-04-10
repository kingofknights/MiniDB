#pragma once
#include "src/catalog/schema.h"
#include <variant>
#include <vector>
#include <string>
#include <cstdint>

namespace minidb {

class Value {
public:
    Value(int32_t val) : data_(val) {}
    Value(std::string val) : data_(std::move(val)) {}

    int32_t AsInt() const { return std::get<int32_t>(data_); }
    const std::string& AsString() const { return std::get<std::string>(data_); }

    DataType GetType() const {
        return std::holds_alternative<int32_t>(data_) ? DataType::INT : DataType::TEXT;
    }

private:
    std::variant<int32_t, std::string> data_;
};

class Record {
public:
    Record(std::vector<Value> values) : values_(std::move(values)) {}

    const std::vector<Value>& GetValues() const { return values_; }
    const Value& GetValue(size_t index) const { return values_[index]; }

    // Serialize record into a byte buffer according to the schema
    static std::vector<uint8_t> Serialize(const Schema& schema, const Record& record);
    
    // Deserialize record from a byte buffer according to the schema
    static Record Deserialize(const Schema& schema, const uint8_t* data, size_t& bytes_read);

private:
    std::vector<Value> values_;
};

} // namespace minidb
