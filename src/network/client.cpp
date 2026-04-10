#include "src/network/client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>

namespace minidb {

Client::Client(const std::string& host, uint16_t port)
    : host_(host), port_(port), sock_fd_(-1) {}

Client::~Client() {
    Disconnect();
}

bool Client::Connect() {
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) return false;

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, host_.c_str(), &serv_addr.sin_addr) <= 0) return false;

    if (connect(sock_fd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) return false;

    return true;
}

std::string Client::SendQuery(const std::string& query) {
    if (sock_fd_ == -1) return "Not connected";

    uint32_t len = query.length();
    send(sock_fd_, &len, 4, 0);
    send(sock_fd_, query.c_str(), len, 0);

    uint32_t resp_len;
    if (recv(sock_fd_, &resp_len, 4, 0) <= 0) return "Connection lost";

    std::vector<char> buffer(resp_len);
    if (recv(sock_fd_, buffer.data(), resp_len, 0) <= 0) return "Connection lost";

    return std::string(buffer.begin(), buffer.end());
}

void Client::Disconnect() {
    if (sock_fd_ != -1) {
        close(sock_fd_);
        sock_fd_ = -1;
    }
}

} // namespace minidb
