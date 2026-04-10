#include "src/network/server.h"
#include "src/parser/lexer.h"
#include "src/parser/parser.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

namespace minidb {

Server::Server(Catalog& catalog, Pager& pager, uint16_t port)
    : catalog_(catalog), pager_(pager), port_(port), server_fd_(-1), running_(false) {}

Server::~Server() {
    Stop();
}

void Server::Start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        perror("socket failed");
        return;
    }

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return;
    }

    if (listen(server_fd_, 3) < 0) {
        perror("listen failed");
        return;
    }

    running_ = true;
    std::cout << "MiniDB Server listening on port " << port_ << std::endl;

    while (running_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            if (running_) perror("accept failed");
            continue;
        }
        HandleClient(client_fd);
        close(client_fd);
    }
}

void Server::Stop() {
    running_ = false;
    if (server_fd_ != -1) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

void Server::HandleClient(int client_fd) {
    while (running_) {
        uint32_t len;
        ssize_t bytes_received = recv(client_fd, &len, 4, 0);
        if (bytes_received <= 0) break;
        
        std::vector<char> buffer(len);
        bytes_received = recv(client_fd, buffer.data(), len, 0);
        if (bytes_received <= 0) break;
        
        std::string query(buffer.begin(), buffer.end());
        std::string result = ProcessQuery(query);
        
        uint32_t resp_len = result.length();
        send(client_fd, &resp_len, 4, 0);
        send(client_fd, result.c_str(), resp_len, 0);
    }
}

std::string Server::ProcessQuery(const std::string& query) {
    std::stringstream out;
    Lexer lexer(query);
    auto tokens = lexer.Tokenize();
    if (tokens.empty() || tokens[0].type == TokenType::END_OF_FILE) {
        return "Empty query\n";
    }

    Parser parser(tokens);
    Status parse_status = Status::OK();
    auto stmt = parser.Parse(parse_status);

    if (!parse_status.ok()) {
        return "Parser Error: " + parse_status.message() + "\n";
    }

    Executor executor(catalog_, pager_);
    Status exec_status = executor.Execute(*stmt, out);
    if (!exec_status.ok()) {
        out << "Execution Error: " << exec_status.message() << "\n";
    }

    return out.str();
}

} // namespace minidb
