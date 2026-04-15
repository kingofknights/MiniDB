#include <gtest/gtest.h>
#include "src/parser/lexer.h"
#include "src/parser/parser.h"
#include "src/execution/executor.h"
#include "src/storage/log_manager.h"
#include <filesystem>

namespace minidb {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "integration_test.db";
        std::filesystem::remove(test_db_);
        std::filesystem::remove("int_test.log");
    }

    void TearDown() override {
        std::filesystem::remove(test_db_);
        std::filesystem::remove("int_test.log");
    }

    Status ExecuteSQL(const std::string& sql, Catalog& catalog, Pager& pager) {
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        Status status = Status::OK();
        auto stmt = parser.Parse(status);
        if (!status.ok()) return status;
        
        LogManager log_manager("int_test.log");
        Executor executor(catalog, pager, log_manager);
        std::stringstream ss;
        return executor.Execute(*stmt, ss);
    }

    std::string test_db_;
};

TEST_F(IntegrationTest, E2EFlow) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage(); // Catalog page
    Catalog catalog;

    ASSERT_TRUE(ExecuteSQL("CREATE TABLE employees (id INT, name TEXT);", catalog, *pager).ok());
    ASSERT_TRUE(ExecuteSQL("INSERT INTO employees VALUES (101, 'Vikram');", catalog, *pager).ok());
    ASSERT_TRUE(ExecuteSQL("INSERT INTO employees VALUES (102, 'Gemini');", catalog, *pager).ok());
    
    ASSERT_TRUE(ExecuteSQL("SELECT * FROM employees;", catalog, *pager).ok());

    // Test WHERE clause
    ASSERT_TRUE(ExecuteSQL("SELECT * FROM employees WHERE id = 101;", catalog, *pager).ok());

    // Test DELETE
    ASSERT_TRUE(ExecuteSQL("DELETE FROM employees WHERE id = 101;", catalog, *pager).ok());
    
    // Verify deletion
    Lexer lexer("SELECT * FROM employees;");
    auto tokens = lexer.Tokenize();
    Parser parser(tokens);
    Status s = Status::OK();
    auto stmt = parser.Parse(s);
    LogManager log_manager("int_test_2.log");
    Executor executor(catalog, *pager, log_manager);
    std::stringstream ss;
    executor.Execute(*stmt, ss);
}

} // namespace minidb
