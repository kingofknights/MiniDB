#pragma once
#include "src/catalog/schema.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace minidb {

/**
 * Catalog manages table metadata.
 * For v1, it keeps metadata in memory but is designed to be persistent.
 */
class Catalog {
public:
    void CreateTable(std::string name, Schema schema) {
        for (auto & c: name) c = std::toupper(c);
        tables_[name] = std::make_unique<Schema>(std::move(schema));
    }

    bool TableExists(std::string name) const {
        for (auto & c: name) c = std::toupper(c);
        return tables_.count(name) > 0;
    }

    const Schema& GetSchema(std::string name) const {
        for (auto & c: name) c = std::toupper(c);
        return *tables_.at(name);
    }

    void Serialize(std::vector<uint8_t>& buffer) const;
    static std::unique_ptr<Catalog> Deserialize(const uint8_t* buffer);

private:
    std::unordered_map<std::string, std::unique_ptr<Schema>> tables_;
};

} // namespace minidb
