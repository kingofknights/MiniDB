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

// --- PAGER TESTS (10 Tests) ---

TEST_F(ExtensiveStorageTest, PagerManyAllocations) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(pager->AllocatePage(), i);
    }
    EXPECT_EQ(pager->GetPageCount(), 100);
}

TEST_F(ExtensiveStorageTest, PagerReadWriteStress) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Page p;
    p.SetPageID(0);
    for (int i = 0; i < 10; ++i) {
        memset(p.GetData(), i, PAGE_SIZE);
        pager->WritePage(p);
        Page p2;
        pager->ReadPage(0, p2);
        EXPECT_EQ(p2.GetData()[0], i);
        EXPECT_EQ(p2.GetData()[PAGE_SIZE-1], i);
    }
}

TEST_F(ExtensiveStorageTest, PagerInvalidPageID) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    Page p;
    EXPECT_FALSE(pager->ReadPage(999, p).ok());
    EXPECT_FALSE(pager->WritePage(p).ok()); // p has INVALID_PAGE_ID
}

TEST_F(ExtensiveStorageTest, PagerReopenPersistence) {
    {
        Status status = Status::OK();
        auto pager = Pager::Open(test_db_, status);
        pager->AllocatePage();
        Page p;
        p.SetPageID(0);
        strcpy((char*)p.GetData(), "Data");
        pager->WritePage(p);
    }
    {
        Status status = Status::OK();
        auto pager = Pager::Open(test_db_, status);
        EXPECT_EQ(pager->GetPageCount(), 1);
        Page p;
        pager->ReadPage(0, p);
        EXPECT_STREQ((char*)p.GetData(), "Data");
    }
}

TEST_F(ExtensiveStorageTest, PagerEmptyFile) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    EXPECT_EQ(pager->GetPageCount(), 0);
}

TEST_F(ExtensiveStorageTest, PagerCloseAndOpen) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->Close();
    auto pager2 = Pager::Open(test_db_, status);
    EXPECT_NE(pager2, nullptr);
}

TEST_F(ExtensiveStorageTest, PagerLargeFileBoundary) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    // Allocate 500 pages (approx 2MB)
    for(int i=0; i<500; ++i) pager->AllocatePage();
    EXPECT_EQ(pager->GetPageCount(), 500);
}

TEST_F(ExtensiveStorageTest, PagerZeroFillOnAllocate) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    PageID id = pager->AllocatePage();
    Page p;
    pager->ReadPage(id, p);
    for(size_t i=0; i<PAGE_SIZE; ++i) {
        EXPECT_EQ(p.GetData()[i], 0);
    }
}

TEST_F(ExtensiveStorageTest, PagerOverwriteSamePage) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    pager->AllocatePage();
    Page p;
    p.SetPageID(0);
    strcpy((char*)p.GetData(), "First");
    pager->WritePage(p);
    strcpy((char*)p.GetData(), "Second");
    pager->WritePage(p);
    Page p2;
    pager->ReadPage(0, p2);
    EXPECT_STREQ((char*)p2.GetData(), "Second");
}

TEST_F(ExtensiveStorageTest, PagerMultipleAllocationsPersistence) {
    {
        Status status = Status::OK();
        auto pager = Pager::Open(test_db_, status);
        for(int i=0; i<5; ++i) pager->AllocatePage();
    }
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    EXPECT_EQ(pager->GetPageCount(), 5);
}

// --- RECORD TESTS (10 Tests) ---

TEST_F(ExtensiveStorageTest, RecordEmptyString) {
    Schema schema({{"col", DataType::TEXT}});
    Record rec({Value("")});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsString(), "");
}

TEST_F(ExtensiveStorageTest, RecordMaxInt) {
    Schema schema({{"col", DataType::INT}});
    Record rec({Value(2147483647)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsInt(), 2147483647);
}

TEST_F(ExtensiveStorageTest, RecordMinInt) {
    Schema schema({{"col", DataType::INT}});
    Record rec({Value(-2147483648)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsInt(), -2147483648);
}

TEST_F(ExtensiveStorageTest, RecordLargeString) {
    Schema schema({{"col", DataType::TEXT}});
    std::string large(2000, 'z');
    Record rec({Value(large)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsString(), large);
}

TEST_F(ExtensiveStorageTest, RecordMultipleColumns) {
    Schema schema({{"c1", DataType::INT}, {"c2", DataType::TEXT}, {"c3", DataType::INT}});
    Record rec({Value(1), Value("abc"), Value(2)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(1).AsString(), "abc");
    EXPECT_EQ(rec2.GetValue(2).AsInt(), 2);
}

TEST_F(ExtensiveStorageTest, RecordDeleteFlagSerialization) {
    Schema schema({{"c1", DataType::INT}});
    Record rec({Value(100)}, true);
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_TRUE(rec2.IsDeleted());
    EXPECT_EQ(rec2.GetValue(0).AsInt(), 100);
}

TEST_F(ExtensiveStorageTest, RecordNotDeletedSerialization) {
    Schema schema({{"c1", DataType::INT}});
    Record rec({Value(100)}, false);
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_FALSE(rec2.IsDeleted());
}

TEST_F(ExtensiveStorageTest, RecordTextWithSpaces) {
    Schema schema({{"c1", DataType::TEXT}});
    Record rec({Value("hello world spaces")});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsString(), "hello world spaces");
}

TEST_F(ExtensiveStorageTest, RecordComplexSchema) {
    Schema schema({
        {"a", DataType::INT}, {"b", DataType::TEXT}, {"c", DataType::INT}, 
        {"d", DataType::TEXT}, {"e", DataType::INT}
    });
    Record rec({Value(1), Value("one"), Value(2), Value("two"), Value(3)});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(3).AsString(), "two");
    EXPECT_EQ(rec2.GetValue(4).AsInt(), 3);
}

TEST_F(ExtensiveStorageTest, RecordAllTypesRoundTrip) {
    Schema schema({{"i", DataType::INT}, {"t", DataType::TEXT}});
    Record rec({Value(42), Value("deep thought")});
    auto buf = Record::Serialize(schema, rec);
    size_t br;
    Record rec2 = Record::Deserialize(schema, buf.data(), br);
    EXPECT_EQ(rec2.GetValue(0).AsInt(), 42);
    EXPECT_EQ(rec2.GetValue(1).AsString(), "deep thought");
}

// --- TABLE HEAP TESTS (10 Tests) ---

TEST_F(ExtensiveStorageTest, TableHeapEmptyScan) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage(); // Catalog
    Schema schema({{"id", DataType::INT}});
    TableHeap table(*pager, schema);
    EXPECT_EQ(table.Scan().size(), 0);
}

TEST_F(ExtensiveStorageTest, TableHeapSingleInsertScan) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"id", DataType::INT}});
    TableHeap table(*pager, schema);
    table.InsertRecord(Record({Value(1)}));
    auto res = table.Scan();
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].GetValue(0).AsInt(), 1);
}

TEST_F(ExtensiveStorageTest, TableHeapManyInserts) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"id", DataType::INT}});
    TableHeap table(*pager, schema);
    for(int i=0; i<100; ++i) table.InsertRecord(Record({Value(i)}));
    EXPECT_EQ(table.Scan().size(), 100);
}

TEST_F(ExtensiveStorageTest, TableHeapScanSkipsPageZero) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage(); // Page 0
    Page p0;
    pager->ReadPage(0, p0);
    memset(p0.GetData(), 0xFF, PAGE_SIZE); // Fill with junk
    pager->WritePage(p0);

    Schema schema({{"id", DataType::INT}});
    TableHeap table(*pager, schema);
    table.InsertRecord(Record({Value(1)}));
    auto res = table.Scan();
    EXPECT_EQ(res.size(), 1); // Should not crash or read junk from P0
}

TEST_F(ExtensiveStorageTest, TableHeapMixedTypes) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"id", DataType::INT}, {"name", DataType::TEXT}});
    TableHeap table(*pager, schema);
    table.InsertRecord(Record({Value(1), Value("Alice")}));
    table.InsertRecord(Record({Value(2), Value("Bob")}));
    auto res = table.Scan();
    EXPECT_EQ(res[1].GetValue(1).AsString(), "Bob");
}

TEST_F(ExtensiveStorageTest, TableHeapRecordTooLarge) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"data", DataType::TEXT}});
    TableHeap table(*pager, schema);
    std::string way_too_large(PAGE_SIZE + 100, 'x');
    table.InsertRecord(Record({Value(way_too_large)}));
}

TEST_F(ExtensiveStorageTest, TableHeapMultiplePages) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"data", DataType::TEXT}});
    TableHeap table(*pager, schema);
    std::string large(1000, 'a');
    for(int i=0; i<10; ++i) table.InsertRecord(Record({Value(large)}));
    EXPECT_GE(pager->GetPageCount(), 3);
}

TEST_F(ExtensiveStorageTest, TableHeapPersistence) {
    Schema schema({{"id", DataType::INT}});
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

TEST_F(ExtensiveStorageTest, TableHeapStressInserts) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"id", DataType::INT}});
    TableHeap table(*pager, schema);
    for(int i=0; i<500; ++i) table.InsertRecord(Record({Value(i)}));
    auto res = table.Scan();
    EXPECT_EQ(res.size(), 500);
}

TEST_F(ExtensiveStorageTest, TableHeapFillPagesExactly) {
    Status s = Status::OK();
    auto pager = Pager::Open(test_db_, s);
    pager->AllocatePage();
    Schema schema({{"data", DataType::TEXT}});
    TableHeap table(*pager, schema);
    std::string chunk(1000, 'x'); 
    for(int i=0; i<10; ++i) table.InsertRecord(Record({Value(chunk)}));
    EXPECT_GT(pager->GetPageCount(), 1);
}

} // namespace minidb
