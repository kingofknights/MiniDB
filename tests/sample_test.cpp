#include <gtest/gtest.h>

TEST(SampleTest, BasicAssertion) {
  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7 * 6, 42);
}
