#include <gtest/gtest.h>
#include "src/storage/pager.h"
#include "src/storage/record.h"
#include "src/storage/table_heap.h"
#include <filesystem>
#include <random>
#include <cstring>

namespace minidb {

class ExtensiveStorageTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "ext_storage_test.db";
        std::filesystem::remove(test_db_);
    }
    void TearDown() override {
        std::filesystem::remove(test_db_);
    }
    std::string test_db_;
};

// --- PAGER TESTS ---

TEST_F(ExtensiveStorageTest, PagerManyAllocations) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    for (int i = 0; i < 50; ++i) {
        EXPECT_EQ(pager->AllocatePage(), i);
    }
}

TEST_F(ExtensiveStorageTest, PagerReadWriteStress) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Page p;
    p.SetPageID(0);
    memset(p.GetData(), 0xAA, PAGE_SIZE);
    pager->WritePage(p);
    Page p2;
    pager->ReadPage(0, p2);
    EXPECT_EQ(p2.GetData()[0], 0xAA);
}

// --- RECORD TESTS ---

TEST_F(ExtensiveStorageTest, RecordEmptyString) {
    Schema schema({Column("col", DataType::TEXT)});
    Record rec({Value("")});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsString(), "");
}

TEST_F(ExtensiveStorageTest, RecordMaxInt) {
    Schema schema({Column("col", DataType::INT)});
    Record rec({Value(2147483647)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsInt(), 2147483647);
}

TEST_F(ExtensiveStorageTest, RecordLargeString) {
    Schema schema({Column("col", DataType::TEXT)});
    std::string large(1000, 'z');
    Record rec({Value(large)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsString(), large);
}

// --- TABLE HEAP TESTS ---

TEST_F(ExtensiveStorageTest, TableHeapSingleInsertScan) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage(); // Catalog
    Schema schema({Column("id", DataType::INT)});
    TableHeap table(*pager, schema);
    table.InsertRecord(Record({Value(1)}));
    auto res = table.Scan();
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].GetValue(0).AsInt(), 1);
}

TEST_F(ExtensiveStorageTest, TableHeapPersistence) {
    Schema schema({Column("id", DataType::INT)});
    {
        Status s = Status::OK();
        auto pager = Pager::Open(test_db_, s);
        pager->AllocatePage();
        TableHeap table(*pager, schema);
        table.InsertRecord(Record({Value(123)}));
    }
    {
        Status s = Status::OK();
        auto pager = Pager::Open(test_db_, s);
        TableHeap table(*pager, schema);
        auto res = table.Scan();
        EXPECT_EQ(res.size(), 1);
        EXPECT_EQ(res[0].GetValue(0).AsInt(), 123);
    }
}

} // namespace minidb
