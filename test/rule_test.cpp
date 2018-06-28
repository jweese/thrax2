#include "phrasalrule.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(RuleTests, Alignment) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  PhrasalRule rule{asp};
  auto a = rule.terminalAlignment();
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
}

TEST(RuleTests, AlignmentWithNT) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0 1-1");
  PhrasalRule rule{asp};
  SpanPair nt{{0, 1}, {0, 1}};
  rule.nts.front() = nt;
  cutPoints(rule.alignment, nt.src.start, nt.src.end);
  auto a = rule.terminalAlignment();
  ASSERT_EQ(1, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
}

TEST(RuleTests, AlignmentWithNT2) {
  auto asp = readAlignedSentencePair<false, false>("a c e\tb d f\t0-0 1-1 2-2");
  PhrasalRule rule{asp};
  SpanPair nt{{1, 2}, {1, 2}};
  rule.nts.front() = nt;
  cutPoints(rule.alignment, nt.src.start, nt.src.end);
  auto a = rule.terminalAlignment();
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0, a.front().src);
  EXPECT_EQ(0, a.front().tgt);
  EXPECT_EQ(1, a.back().src);
  EXPECT_EQ(1, a.back().tgt);
}

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

}
