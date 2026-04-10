#pragma once
#include "src/execution/executor.h"
#include <string>
#include <vector>
#include <netinet/in.h>

namespace minidb {

class Server {
public:
    Server(Catalog& catalog, Pager& pager, uint16_t port);
    ~Server();

    // Start the server and listen for connections (blocking)
    void Start();

    // Stop the server
    void Stop();

private:
    void HandleClient(int client_fd);
    std::string ProcessQuery(const std::string& query);

    Catalog& catalog_;
    Pager& pager_;
    uint16_t port_;
    int server_fd_;
    bool running_;
};

} // namespace minidb
