#include <gtest/gtest.h>
#include "src/parser/lexer.h"
#include "src/parser/parser.h"
#include "src/execution/executor.h"
#include "src/storage/log_manager.h"
#include <filesystem>

namespace minidb {

class IndexTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "index_test.db";
        std::filesystem::remove(test_db_);
    }

    void TearDown() override {
        std::filesystem::remove(test_db_);
    }

    Status ExecuteSQL(const std::string& sql, Catalog& catalog, Pager& pager) {
        LogManager log_manager("index_test.log");
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        Status status = Status::OK();
        auto stmt = parser.Parse(status);
        if (!status.ok()) return status;
        
        Executor executor(catalog, pager, log_manager);
        std::stringstream ss;
        return executor.Execute(*stmt, ss);
    }

    std::string test_db_;
};

TEST_F(IndexTest, CreateAndUseIndex) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage(); // Catalog page
    Catalog catalog;

    ASSERT_TRUE(ExecuteSQL("CREATE TABLE users (id INT, name TEXT);", catalog, *pager).ok());
    ASSERT_TRUE(ExecuteSQL("INSERT INTO users VALUES (1, 'Alice');", catalog, *pager).ok());
    ASSERT_TRUE(ExecuteSQL("INSERT INTO users VALUES (2, 'Bob');", catalog, *pager).ok());

    // Create Index
    ASSERT_TRUE(ExecuteSQL("CREATE INDEX id_idx ON users (id);", catalog, *pager).ok());

    // Execute SELECT which should use index
    ASSERT_TRUE(ExecuteSQL("SELECT * FROM users WHERE id = 1;", catalog, *pager).ok());
    
    // Insert more data - should update index
    ASSERT_TRUE(ExecuteSQL("INSERT INTO users VALUES (3, 'Charlie');", catalog, *pager).ok());
    ASSERT_TRUE(ExecuteSQL("SELECT * FROM users WHERE id = 3;", catalog, *pager).ok());
}

} // namespace minidb
