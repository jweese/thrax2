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

}
