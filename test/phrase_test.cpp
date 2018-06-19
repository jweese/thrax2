#include "phrase.h"
#include "sentence.h"
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
  SpanPair sp1{{0, 1}, {0, 1}};
  SpanPair sp2{{1, 2}, {1, 2}};
  SpanPair sp3{{0, 2}, {0, 2}};
  EXPECT_TRUE(isConsistent(a, sp1));
  EXPECT_TRUE(isConsistent(a, sp2));
  EXPECT_TRUE(isConsistent(a, sp3));
}

TEST(PhraseTests, Inconsistent) {
  auto a = readAlignment("0-0 1-2 2-1");
  SpanPair sp{{0, 1}, {0, 3}};
  EXPECT_FALSE(isConsistent(a, sp));
}

TEST(PhraseTests, AllConsistentPairsFromEmpty) {
  auto a = readAlignment("");
  auto sps = minimalConsistentPairs(a, 0);
  EXPECT_EQ(0, sps.size());
}

TEST(PhraseTests, AllConsistentPairsSizeOne) {
  auto a = readAlignment("0-0 1-1");
  auto sps = minimalConsistentPairs(a, 1);
  auto f = sps.front();
  EXPECT_EQ(2, sps.size());
}

TEST(PhraseTests, AllConsistentPairsSizeTwo) {
  auto a = readAlignment("0-0 1-1");
  auto sps = minimalConsistentPairs(a, 2);
  auto f = sps.front();
  EXPECT_EQ(3, sps.size());
}

TEST(PhraseTests, Disjoint) {
  EXPECT_TRUE(disjoint(Span{0, 0}, {0, 1}));
  EXPECT_TRUE(disjoint(Span{0, 0}, {1, 1}));
  EXPECT_TRUE(disjoint(Span{0, 1}, {1, 2}));
  EXPECT_TRUE(disjoint(Span{0, 1}, {1, 1}));
  EXPECT_TRUE(disjoint(Span{1, 1}, {0, 1}));
  EXPECT_FALSE(disjoint(Span{0, 1}, {0, 2}));
  EXPECT_FALSE(disjoint(Span{0, 4}, {1, 2}));
  EXPECT_FALSE(disjoint(Span{3, 4}, {0, 4}));
}

TEST(PhraseTests, HasValue) {
  EXPECT_TRUE(Span{}.empty());
  EXPECT_TRUE(SpanPair{}.empty());
  Span s{1, 1};
  EXPECT_TRUE(s.empty());
  s.end++;
  EXPECT_FALSE(s.empty());
}

TEST(PhraseTests, SpanIndices) {
  Span s{0, 3};
  auto is = s.indices();
  ASSERT_EQ(3, is.size());
  for (int i = 0; i < is.size(); i++) {
    EXPECT_EQ(s.start + i, is[i]);
  }
}

TEST(PhraseTests, RemoveIndices) {
  Span s{0, 3};
  auto is = s.indices();
  removeIndices(is, {0, 1});
  ASSERT_EQ(2, is.size());
  EXPECT_EQ(1, is.front());
  EXPECT_EQ(2, is.back());
}

TEST(PhraseTests, AllPairs) {
  auto a = readAlignment("0-0 2-1 2-2 2-3");
  auto ps = minimalConsistentPairs(a, 10);
  ASSERT_EQ(3, ps.size());
}

TEST(PhraseTests, LoosePhrases) {
  auto asp = readAlignedSentencePair<false, false>("a b c\td e f\t1-1");
  auto ps = allConsistentPairs(asp, 10);
  ASSERT_EQ(16, ps.size());
}

}
