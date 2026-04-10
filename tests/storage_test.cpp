#include <gtest/gtest.h>
#include "src/storage/pager.h"
#include <filesystem>
#include <cstring>

namespace minidb {

class PagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_db_ = "test.db";
        std::filesystem::remove(test_db_);
    }

    void TearDown() override {
        std::filesystem::remove(test_db_);
    }

    std::string test_db_;
};

TEST_F(PagerTest, OpenAndCreate) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    ASSERT_TRUE(status.ok());
    ASSERT_NE(pager, nullptr);
    EXPECT_EQ(pager->GetPageCount(), 0);
}

TEST_F(PagerTest, AllocateAndWriteRead) {
    Status status = Status::OK();
    auto pager = Pager::Open(test_db_, status);
    
    PageID p0 = pager->AllocatePage();
    EXPECT_EQ(p0, 0);
    EXPECT_EQ(pager->GetPageCount(), 1);

    Page write_page;
    write_page.SetPageID(p0);
    std::strcpy(reinterpret_cast<char*>(write_page.GetData()), "Hello Pager!");
    
    status = pager->WritePage(write_page);
    ASSERT_TRUE(status.ok());

    Page read_page;
    status = pager->ReadPage(p0, read_page);
    ASSERT_TRUE(status.ok());
    EXPECT_STREQ(reinterpret_cast<const char*>(read_page.GetData()), "Hello Pager!");
}

TEST_F(PagerTest, Persistence) {
    {
        Status status = Status::OK();
        auto pager = Pager::Open(test_db_, status);
        PageID p0 = pager->AllocatePage();
        Page write_page;
        write_page.SetPageID(p0);
        std::memcpy(write_page.GetData(), "Persistent Data", 16);
        pager->WritePage(write_page);
    } // Pager closes here

    {
        Status status = Status::OK();
        auto pager = Pager::Open(test_db_, status);
        EXPECT_EQ(pager->GetPageCount(), 1);
        Page read_page;
        pager->ReadPage(0, read_page);
        EXPECT_EQ(std::memcmp(read_page.GetData(), "Persistent Data", 16), 0);
    }
}

} // namespace minidb
