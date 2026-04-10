#pragma once
#include <string>
#include <cstdint>

namespace minidb {

class Client {
public:
    Client(const std::string& host, uint16_t port);
    ~Client();

    bool Connect();
    std::string SendQuery(const std::string& query);
    void Disconnect();

private:
    std::string host_;
    uint16_t port_;
    int sock_fd_;
};

} // namespace minidb
