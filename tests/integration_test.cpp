#include <gtest/gtest.h>
#include "src/parser/lexer.h"
#include "src/parser/parser.h"
#include "src/execution/executor.h"
#include <filesystem>

namespace minidb {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "integration_test.db";
        std::filesystem::remove(test_db_);
    }

    void TearDown() override {
        std::filesystem::remove(test_db_);
    }

    Status ExecuteSQL(const std::string& sql, Catalog& catalog, Pager& pager) {
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        Status status = Status::OK();
        auto stmt = parser.Parse(status);
        if (!status.ok()) return status;
        
        Executor executor(catalog, pager);
        return executor.Execute(*stmt);
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
    
    // Capture stdout would be better, but for now we just check if it runs without error
    ASSERT_TRUE(ExecuteSQL("SELECT * FROM employees;", catalog, *pager).ok());
}

} // namespace minidb
