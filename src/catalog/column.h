#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace minidb {

enum class DataType {
    INT,
    TEXT
};

class Column {
public:
    Column(std::string name, DataType type) : name_(std::move(name)), type_(type) {
        for (auto & c: name_) c = std::toupper(c);
    }

    const std::string& GetName() const { return name_; }
    DataType GetType() const { return type_; }

    void Serialize(std::vector<uint8_t>& buffer) const;
    static Column Deserialize(const uint8_t* buffer, size_t& offset);

private:
    std::string name_;
    DataType type_;
};

} // namespace minidb
