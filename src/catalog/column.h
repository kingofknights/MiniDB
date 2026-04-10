#pragma once
#include <string>

namespace minidb {

enum class DataType {
    INT,
    TEXT
};

class Column {
public:
    Column(std::string name, DataType type) : name_(std::move(name)), type_(type) {}

    const std::string& GetName() const { return name_; }
    DataType GetType() const { return type_; }

private:
    std::string name_;
    DataType type_;
};

} // namespace minidb
