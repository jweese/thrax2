#include "phrasalrule.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(RuleTests, RemainingSpanLexical) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  Span s{0, 3};
  EXPECT_EQ(s, rule.remainingSource());
}

TEST(RuleTests, RemainingSpanWithNT) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  SpanPair nt{{1, 2}, {1, 2}};
  rule.nts.front() = nt;
  rule.nextNT++;
  Span s{2, 3};
  EXPECT_EQ(s, rule.remainingSource());
}

TEST(RuleTests, numSourceTokens) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  SpanPair nt{{1, 2}, {1, 2}};
  rule.nts.front() = nt;
  EXPECT_EQ(3, rule.numTokens<true>());
}

TEST(RuleTests, numTargetTokens) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  SpanPair nt{{1, 2}, {1, 2}};
  rule.nts.front() = nt;
  EXPECT_EQ(3, rule.numTokens<false>());
}

TEST(RuleTests, numSourceTokensLex) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  EXPECT_EQ(3, rule.numTokens<true>());
}

TEST(RuleTests, numTargetTokensLex) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  EXPECT_EQ(3, rule.numTokens<false>());
}

}
