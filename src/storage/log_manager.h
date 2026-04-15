#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>

namespace minidb {

enum class LogRecordType {
    BEGIN,
    COMMIT,
    ROLLBACK,
    UPDATE,
    INSERT,
    DELETE
};

struct LogRecord {
    uint32_t lsn; // Log Sequence Number
    LogRecordType type;
    std::string table_name;
    // For simplicity, we store raw serialized before/after for redo/undo
    std::vector<uint8_t> before_image;
    std::vector<uint8_t> after_image;
};

class LogManager {
public:
    LogManager(const std::string& log_file) : filename_(log_file), next_lsn_(0) {
        log_stream_.open(filename_, std::ios::out | std::ios::app | std::ios::binary);
    }

    // ... AppendLog ...

    std::vector<LogRecord> ReadAllLogs() {
        std::vector<LogRecord> records;
        std::ifstream in(filename_, std::ios::in | std::ios::binary);
        if (!in.is_open()) return records;

        while (in.peek() != EOF) {
            LogRecord rec;
            in.read(reinterpret_cast<char*>(&rec.lsn), 4);
            uint8_t t;
            in.read(reinterpret_cast<char*>(&t), 1);
            rec.type = static_cast<LogRecordType>(t);

            uint32_t name_len;
            in.read(reinterpret_cast<char*>(&name_len), 4);
            std::vector<char> name_buf(name_len);
            in.read(name_buf.data(), name_len);
            rec.table_name = std::string(name_buf.begin(), name_buf.end());

            uint32_t before_len;
            in.read(reinterpret_cast<char*>(&before_len), 4);
            if (before_len > 0) {
                rec.before_image.resize(before_len);
                in.read(reinterpret_cast<char*>(rec.before_image.data()), before_len);
            }

            uint32_t after_len;
            in.read(reinterpret_cast<char*>(&after_len), 4);
            if (after_len > 0) {
                rec.after_image.resize(after_len);
                in.read(reinterpret_cast<char*>(rec.after_image.data()), after_len);
            }
            records.push_back(std::move(rec));
        }
        return records;
    }

    uint32_t AppendLog(LogRecordType type, const std::string& table_name = "", 
                       const std::vector<uint8_t>& before = {}, 
                       const std::vector<uint8_t>& after = {}) {
        uint32_t lsn = next_lsn_++;
        // Write to file (simplified binary format)
        log_stream_.write(reinterpret_cast<const char*>(&lsn), 4);
        uint8_t t = static_cast<uint8_t>(type);
        log_stream_.write(reinterpret_cast<const char*>(&t), 1);
        
        uint32_t name_len = table_name.length();
        log_stream_.write(reinterpret_cast<const char*>(&name_len), 4);
        log_stream_.write(table_name.c_str(), name_len);

        uint32_t before_len = before.size();
        log_stream_.write(reinterpret_cast<const char*>(&before_len), 4);
        if (before_len > 0) log_stream_.write(reinterpret_cast<const char*>(before.data()), before_len);

        uint32_t after_len = after.size();
        log_stream_.write(reinterpret_cast<const char*>(&after_len), 4);
        if (after_len > 0) log_stream_.write(reinterpret_cast<const char*>(after.data()), after_len);

        log_stream_.flush();
        return lsn;
    }

private:
    std::string filename_;
    std::ofstream log_stream_;
    uint32_t next_lsn_;
};

} // namespace minidb
