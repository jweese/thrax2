#include "phrase.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(PhraseTests, UnalignedSpan) {
  Alignment a;
  Span src{0, 4};
  EXPECT_FALSE(minimalTargetSpan(a, src).has_value());
}

TEST(PhraseTests, AlignedSpan) {
  auto a = readAlignment("0-0 0-4");
  Span src{0, 1};
  auto tgt = minimalTargetSpan(a, src);
  ASSERT_TRUE(tgt.has_value());
  EXPECT_EQ(0, tgt->start);
  EXPECT_EQ(5, tgt->end);
}

TEST(PhraseTests, ExtraPoints) {
  auto a = readAlignment("0-0 0-4 2-4 3-3");
  Span src{0, 1};
  auto tgt = minimalTargetSpan(a, src);
  ASSERT_TRUE(tgt.has_value());
  EXPECT_EQ(0, tgt->start);
  EXPECT_EQ(5, tgt->end);
}

TEST(PhraseTests, EmptyConsistent) {
  auto a = readAlignment("");
  SpanPair sp{{0, 1}, {3, 6}};
  EXPECT_TRUE(isConsistent(a, sp));
}

TEST(PhraseTests, SmallConsistent) {
  auto a = readAlignment("0-0 1-1");
  SpanPair sp1{{0, 0}, {0, 0}};
  SpanPair sp2{{1, 1}, {1, 1}};
  SpanPair sp3{{0, 1}, {0, 1}};
  EXPECT_TRUE(isConsistent(a, sp1));
  EXPECT_TRUE(isConsistent(a, sp2));
  EXPECT_TRUE(isConsistent(a, sp3));
}

TEST(PhraseTests, Inconsistent) {
  auto a = readAlignment("0-0 1-2 2-1");
  SpanPair sp{{0, 0}, {0, 2}};
  EXPECT_FALSE(isConsistent(a, sp));
}

}
