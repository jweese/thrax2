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

}
