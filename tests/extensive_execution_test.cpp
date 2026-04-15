#include <gtest/gtest.h>
#include "src/execution/executor.h"
#include "src/parser/lexer.h"
#include "src/parser/parser.h"
#include "src/common/status.h"
#include "src/storage/index.h"
#include "src/storage/log_manager.h"
#include <filesystem>

namespace minidb {

class ExtensiveExecutionTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "ext_exec_test.db";
        std::filesystem::remove(test_db_);
    }
    void TearDown() override {
        std::filesystem::remove(test_db_);
    }

    Status RunSQL(const std::string& sql, Catalog& catalog, Pager& pager) {
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        Status s = Status::OK();
        auto stmt = parser.Parse(s);
        if (!s.ok()) return s;
        LogManager log_manager("exec_test.log");
        Executor executor(catalog, pager, log_manager);
        std::stringstream ss;
        return executor.Execute(*stmt, ss);
    }

    std::string test_db_;
};

TEST_F(ExtensiveExecutionTest, CatalogTableNormalization) {
    Catalog catalog;
    catalog.CreateTable("users", Schema({Column("id", DataType::INT)}));
    EXPECT_TRUE(catalog.TableExists("USERS"));
}

TEST_F(ExtensiveExecutionTest, ExecForeignKeyConstraint) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    LogManager log_manager("fk_test.log");
    Executor executor(catalog, *pager, log_manager);
    std::stringstream ss;

    auto exec = [&](const std::string& sql) {
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        Status s = Status::OK();
        auto stmt = parser.Parse(s);
        if (!s.ok()) return s;
        return executor.Execute(*stmt, ss);
    };

    // 1. Create Parent Table
    ASSERT_TRUE(exec("CREATE TABLE DEPARTMENTS (ID INT, NAME TEXT);").ok());
    ASSERT_TRUE(exec("INSERT INTO DEPARTMENTS VALUES (1, 'Engineering');").ok());

    // 2. Create Child Table with FK
    ASSERT_TRUE(exec("CREATE TABLE EMPLOYEES (ID INT, NAME TEXT, DEPT_ID INT, FOREIGN KEY (DEPT_ID) REFERENCES DEPARTMENTS(ID));").ok());

    // 3. Test Invalid Insert (Parent key doesn't exist)
    EXPECT_FALSE(exec("INSERT INTO EMPLOYEES VALUES (101, 'Vikram', 99);").ok());

    // 4. Test Valid Insert
    EXPECT_TRUE(exec("INSERT INTO EMPLOYEES VALUES (101, 'Vikram', 1);").ok());

    // 5. Test Invalid Delete (Parent has children)
    EXPECT_FALSE(exec("DELETE FROM DEPARTMENTS WHERE ID = 1;").ok());

    // 6. Test Valid Delete (Delete child first)
    EXPECT_TRUE(exec("DELETE FROM EMPLOYEES WHERE ID = 101;").ok());
    EXPECT_TRUE(exec("DELETE FROM DEPARTMENTS WHERE ID = 1;").ok());
}

} // namespace minidb
