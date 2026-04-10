#pragma once
#include <string>

namespace minidb {

enum class StatusCode {
    OK,
    IO_ERROR,
    INVALID_PAGE_ID,
    OUT_OF_MEMORY
};

class Status {
public:
    Status(StatusCode code, std::string message = "") : code_(code), message_(std::move(message)) {}

    static Status OK() { return Status(StatusCode::OK); }
    static Status IOError(std::string msg) { return Status(StatusCode::IO_ERROR, std::move(msg)); }

    bool ok() const { return code_ == StatusCode::OK; }
    StatusCode code() const { return code_; }
    const std::string& message() const { return message_; }

private:
    StatusCode code_;
    std::string message_;
};

} // namespace minidb
