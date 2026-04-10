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
        tables_[name] = std::make_unique<Schema>(std::move(schema));
    }

    bool TableExists(const std::string& name) const {
        return tables_.count(name) > 0;
    }

    const Schema& GetSchema(const std::string& name) const {
        return *tables_.at(name);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Schema>> tables_;
};

} // namespace minidb
