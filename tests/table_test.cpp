#include <gtest/gtest.h>
#include "src/storage/table_heap.h"
#include <filesystem>

namespace minidb {

class TableHeapTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "table_test.db";
        std::filesystem::remove(test_db_);
    }

    void TearDown() override {
        std::filesystem::remove(test_db_);
    }

    std::string test_db_;
};

TEST_F(TableHeapTest, InsertAndScan) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage(); // Catalog page
    Schema schema({
        {"id", DataType::INT},
        {"name", DataType::TEXT}
    });
    
    TableHeap table(*pager, schema);

    table.InsertRecord(Record(std::vector<Value>{Value(1), Value("Alice")}));
    table.InsertRecord(Record(std::vector<Value>{Value(2), Value("Bob")}));

    auto records = table.Scan();
    ASSERT_EQ(records.size(), 2);
    EXPECT_EQ(records[0].GetValue(0).AsInt(), 1);
    EXPECT_STREQ(records[0].GetValue(1).AsString().c_str(), "Alice");
    EXPECT_EQ(records[1].GetValue(0).AsInt(), 2);
    EXPECT_STREQ(records[1].GetValue(1).AsString().c_str(), "Bob");
}

TEST_F(TableHeapTest, MultiPageInsert) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage(); // Catalog page
    
    // Schema with a large text field to force multi-page usage
    Schema schema({{"id", DataType::INT}, {"data", DataType::TEXT}});
    TableHeap table(*pager, schema);

    // Each record is roughly 1000 + 4 + 4 bytes. 4 records per 4KB page.
    std::string large_str(1000, 'x');
    for (int i = 0; i < 10; ++i) {
        table.InsertRecord(Record(std::vector<Value>{Value(i), Value(large_str)}));
    }

    // Should have used at least 3 pages
    EXPECT_GE(pager->GetPageCount(), 3);

    auto records = table.Scan();
    EXPECT_EQ(records.size(), 10);
    EXPECT_EQ(records[9].GetValue(0).AsInt(), 9);
}

} // namespace minidb
