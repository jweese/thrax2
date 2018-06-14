#include "phrasalrule.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(RuleTests, Indices) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  PhrasalRule rule{asp};
  auto is = rule.terminalIndices<true>();
  ASSERT_EQ(1, is.size());
  EXPECT_EQ(0, is.front());
  is = rule.terminalIndices<false>();
  ASSERT_EQ(1, is.size());
  EXPECT_EQ(0, is.front());
}

TEST(RuleTests, IndicesNonDefault) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  PhrasalRule rule{asp, {{ 1, 2}, {0, 1}}};
  auto is = rule.terminalIndices<true>();
  ASSERT_EQ(1, is.size());
  EXPECT_EQ(1, is.front());
  is = rule.terminalIndices<false>();
  ASSERT_EQ(1, is.size());
  EXPECT_EQ(0, is.front());
}

TEST(RuleTests, Alignment) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  PhrasalRule rule{asp};
  auto a = rule.alignment();
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
}

TEST(RuleTests, AlignmentWithNT) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0 1-1");
  PhrasalRule rule{asp};
  rule.nts.front() = {{0, 1}, {0, 1}};
  auto a = rule.alignment();
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
}

TEST(RuleTests, AlignmentWithNT2) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  rule.nts.front() = {{1, 2}, {1, 2}};
  auto a = rule.alignment();
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
  EXPECT_EQ(1, a.back().src);
  EXPECT_EQ(1, a.back().tgt);
}

}
