#include <gtest/gtest.h>
#include "src/common/status.h"
#include "src/storage/record.h"

namespace minidb {

TEST(CommonTest, StatusOK) {
    Status s = Status::OK();
    EXPECT_TRUE(s.ok());
    EXPECT_EQ(s.code(), StatusCode::OK);
}

TEST(CommonTest, StatusError) {
    Status s = Status::IOError("fail");
    EXPECT_FALSE(s.ok());
    EXPECT_EQ(s.code(), StatusCode::IO_ERROR);
    EXPECT_EQ(s.message(), "fail");
}

TEST(CommonTest, ValueInt) {
    Value v(123);
    EXPECT_EQ(v.GetType(), DataType::INT);
    EXPECT_EQ(v.AsInt(), 123);
}

TEST(CommonTest, ValueText) {
    Value v("hello");
    EXPECT_EQ(v.GetType(), DataType::TEXT);
    EXPECT_EQ(v.AsString(), "hello");
}

TEST(CommonTest, ValueAssignment) {
    Value v1(10);
    Value v2 = v1;
    EXPECT_EQ(v2.AsInt(), 10);
}

TEST(CommonTest, ValueMove) {
    Value v1("move me");
    Value v2 = std::move(v1);
    EXPECT_EQ(v2.AsString(), "move me");
}

} // namespace minidb
