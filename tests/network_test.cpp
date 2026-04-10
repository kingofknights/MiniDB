#include <gtest/gtest.h>
#include "src/network/server.h"
#include "src/network/client.h"
#include <thread>
#include <filesystem>

namespace minidb {

class NetworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "network_test.db";
        std::filesystem::remove(test_db_);
    }
    void TearDown() override {
        std::filesystem::remove(test_db_);
    }
    std::string test_db_;
};

TEST_F(NetworkTest, EndToEndRemoteQuery) {
    uint16_t port = 5555;
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;

    // Start server in a separate thread
    Server server(catalog, *pager, port);
    std::thread server_thread([&server]() {
        server.Start();
    });

    // Give server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Client connection
    Client client("127.0.0.1", port);
    ASSERT_TRUE(client.Connect());

    // Send queries
    std::string res1 = client.SendQuery("CREATE TABLE remote_users (id INT, name TEXT);");
    EXPECT_TRUE(res1.find("Table created") != std::string::npos);

    std::string res2 = client.SendQuery("INSERT INTO remote_users VALUES (1, 'RemoteUser');");
    EXPECT_TRUE(res2.find("1 row inserted") != std::string::npos);

    std::string res3 = client.SendQuery("SELECT * FROM remote_users;");
    EXPECT_TRUE(res3.find("RemoteUser") != std::string::npos);

    client.Disconnect();
    server.Stop();
    
    // Connect one more time to unblock accept() if needed
    // or just rely on server.Stop() closing the socket.
    // In our simple server, Stop() closes the fd which causes accept to return error.
    
    server_thread.join();
}

} // namespace minidb
