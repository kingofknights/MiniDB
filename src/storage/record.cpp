#include "src/storage/record.h"
#include <cstring>
#include <stdexcept>

namespace minidb {

std::vector<uint8_t> Record::Serialize(const Schema& schema, const Record& record) {
    std::vector<uint8_t> buffer;
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        const auto& col = schema.GetColumn(i);
        const auto& val = record.GetValue(i);

        if (col.GetType() == DataType::INT) {
            int32_t raw_val = val.AsInt();
            uint8_t bytes[4];
            std::memcpy(bytes, &raw_val, 4);
            buffer.insert(buffer.end(), bytes, bytes + 4);
        } else if (col.GetType() == DataType::TEXT) {
            const std::string& str = val.AsString();
            uint32_t len = static_cast<uint32_t>(str.length());
            uint8_t len_bytes[4];
            std::memcpy(len_bytes, &len, 4);
            buffer.insert(buffer.end(), len_bytes, len_bytes + 4);
            buffer.insert(buffer.end(), str.begin(), str.end());
        }
    }
    return buffer;
}

Record Record::Deserialize(const Schema& schema, const uint8_t* data, size_t& bytes_read) {
    size_t offset = 0;
    std::vector<Value> values;
    for (size_t i = 0; i < schema.GetColumnCount(); ++i) {
        const auto& col = schema.GetColumn(i);
        if (col.GetType() == DataType::INT) {
            int32_t val;
            std::memcpy(&val, data + offset, 4);
            values.emplace_back(val);
            offset += 4;
        } else if (col.GetType() == DataType::TEXT) {
            uint32_t len;
            std::memcpy(&len, data + offset, 4);
            offset += 4;
            std::string str(reinterpret_cast<const char*>(data + offset), len);
            values.emplace_back(std::move(str));
            offset += len;
        }
    }
    bytes_read = offset;
    return Record(std::move(values));
}

} // namespace minidb

