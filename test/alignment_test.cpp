#include "alignment.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(AlignmentTests, EmptyString) {
  auto a = readAlignment("");
  EXPECT_EQ(0, a.size());
}

TEST(AlignmentTests, SinglePoint) {
  auto a = readAlignment("0-0");
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
}

TEST(AlignmentTests, PositiveTarget) {
  auto a = readAlignment("0-3");
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(3, a.front().tgt);
}

TEST(AlignmentTests, LeadingZero) {
  auto a = readAlignment("0-011");
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(11, a.front().tgt);
}

TEST(AlignmentTests, MultiPoint) {
  auto a = readAlignment("0-3 7-7");
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(3, a.front().tgt);
  EXPECT_EQ(7, a.back().src);
  EXPECT_EQ(7, a.back().tgt);
}

TEST(AlignmentTests, TrimFront) {
  auto a = readAlignment("   0-3 7-7");
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(3, a.front().tgt);
  EXPECT_EQ(7, a.back().src);
  EXPECT_EQ(7, a.back().tgt);
}

TEST(AlignmentTests, TrimBack) {
  auto a = readAlignment("0-3 7-7    ");
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(3, a.front().tgt);
  EXPECT_EQ(7, a.back().src);
  EXPECT_EQ(7, a.back().tgt);
}

TEST(AlignmentTests, PadCenter) {
  auto a = readAlignment("0-3       7-7");
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(3, a.front().tgt);
  EXPECT_EQ(7, a.back().src);
  EXPECT_EQ(7, a.back().tgt);
}

TEST(AlignmentTests, TooShort) {
  EXPECT_THROW(readAlignment("3-"), std::invalid_argument);
}

TEST(AlignmentTests, NoSource) {
  EXPECT_THROW(readAlignment("-3"), std::invalid_argument);
}

TEST(AlignmentTests, Sort) {
  auto a = readAlignment("3-2 0-0");
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
  EXPECT_EQ(3, a.back().src);
  EXPECT_EQ(2, a.back().tgt);

}

}
