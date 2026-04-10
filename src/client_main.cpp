#include "src/network/client.h"
#include <iostream>

using namespace minidb;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: minidb_client <host> <query>" << std::endl;
        return 1;
    }

    std::string host = argv[1];
    std::string query = argv[2];
    uint16_t port = 5432;

    Client client(host, port);
    if (!client.Connect()) {
        std::cerr << "Could not connect to server at " << host << ":" << port << std::endl;
        return 1;
    }

    std::string result = client.SendQuery(query);
    std::cout << result << std::endl;

    client.Disconnect();
    return 0;
}
