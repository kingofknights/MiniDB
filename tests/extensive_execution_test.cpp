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
        std::filesystem::remove("test.log");
    }
    void TearDown() override {
        std::filesystem::remove(test_db_);
        std::filesystem::remove("test.log");
    }

    Status RunSQL(const std::string& sql, Catalog& catalog, Pager& pager) {
        LogManager log_manager("test.log");
        Lexer lexer(sql);
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        Status s = Status::OK();
        auto stmt = parser.Parse(s);
        if (!s.ok()) return s;
        Executor executor(catalog, pager, log_manager);
        std::stringstream ss;
        return executor.Execute(*stmt, ss);
    }

    std::string test_db_;
};

// --- CATALOG TESTS (4 Tests) ---

TEST_F(ExtensiveExecutionTest, CatalogTableNormalization) {
    Catalog catalog;
    catalog.CreateTable("users", Schema({{"id", DataType::INT}}));
    EXPECT_TRUE(catalog.TableExists("USERS"));
    EXPECT_TRUE(catalog.TableExists("users"));
    EXPECT_TRUE(catalog.TableExists("uSeRs"));
}

TEST_F(ExtensiveExecutionTest, CatalogDuplicateTable) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    EXPECT_TRUE(RunSQL("CREATE TABLE t1 (c1 INT);", catalog, *pager).ok());
    EXPECT_FALSE(RunSQL("CREATE TABLE t1 (c1 INT);", catalog, *pager).ok());
}

TEST_F(ExtensiveExecutionTest, CatalogGetNonExistentTable) {
    Catalog catalog;
    EXPECT_FALSE(catalog.TableExists("MISSING"));
}

TEST_F(ExtensiveExecutionTest, CatalogIndexStorage) {
    Catalog catalog;
    catalog.CreateTable("T1", Schema({{"C1", DataType::INT}}));
    catalog.AddIndex("IDX1", "T1", "C1", IndexType::HASH);
    auto idxs = catalog.GetHashIndexes("T1");
    ASSERT_EQ(idxs.size(), 1);
    EXPECT_EQ(idxs[0]->GetColumnName(), "C1");
}

// --- EXECUTOR & INDEX TESTS ---

TEST_F(ExtensiveExecutionTest, ExecSelectNoData) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    RunSQL("CREATE TABLE t1 (c1 INT);", catalog, *pager);
    EXPECT_TRUE(RunSQL("SELECT * FROM t1;", catalog, *pager).ok());
}

TEST_F(ExtensiveExecutionTest, ExecDeleteAll) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    RunSQL("CREATE TABLE t1 (id INT);", catalog, *pager);
    RunSQL("INSERT INTO t1 VALUES (1);", catalog, *pager);
    RunSQL("INSERT INTO t1 VALUES (2);", catalog, *pager);
    EXPECT_TRUE(RunSQL("DELETE FROM t1;", catalog, *pager).ok());
}

TEST_F(ExtensiveExecutionTest, ExecIndexUsageInSelect) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    RunSQL("CREATE TABLE t1 (id INT);", catalog, *pager);
    RunSQL("CREATE INDEX id_hash_idx ON t1 (id);", catalog, *pager); // Contains HASH
    RunSQL("INSERT INTO t1 VALUES (1);", catalog, *pager);
    EXPECT_TRUE(RunSQL("SELECT * FROM t1 WHERE id = 1;", catalog, *pager).ok());
}

TEST_F(ExtensiveExecutionTest, ExecBTreeRangeQuery) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    RunSQL("CREATE TABLE t1 (id INT);", catalog, *pager);
    RunSQL("CREATE INDEX id_btree ON t1 (id);", catalog, *pager); // Default to BTREE
    for(int i=0; i<10; ++i) RunSQL("INSERT INTO t1 VALUES (" + std::to_string(i) + ");", catalog, *pager);
    
    EXPECT_TRUE(RunSQL("SELECT * FROM t1 WHERE id > 5;", catalog, *pager).ok());
    EXPECT_TRUE(RunSQL("SELECT * FROM t1 WHERE id < 3;", catalog, *pager).ok());
    EXPECT_TRUE(RunSQL("SELECT * FROM t1 WHERE id >= 8;", catalog, *pager).ok());
}

TEST_F(ExtensiveExecutionTest, ExecTableNotFound) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Catalog catalog;
    EXPECT_FALSE(RunSQL("SELECT * FROM missing;", catalog, *pager).ok());
}

} // namespace minidb
