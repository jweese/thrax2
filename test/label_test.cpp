#include "label.h"
#include "phrasalrule.h"
#include "gtest/gtest.h"

#include <sstream>

namespace jhu::thrax {

TEST(LabelTests, Hiero) {
  SpanPair sp;
  HieroLabeler hl;
  ASSERT_EQ("X", hl(sp));
}

TEST(LabelTests, PrintHieroLexical) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  PhrasalRule rule{asp};
  std::ostringstream s;
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X]\ta\tb\t0-0", s.str());
}

TEST(LabelTests, PrintHieroLexicalMulti) {
  auto asp = readAlignedSentencePair<false, false>("a\tb c d\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X]\ta\tb c d\t0-0", s.str());
}

TEST(LabelTests, PrintHieroUnary) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts.front() = asp.span();
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X]\t[X,1]\t[X,1]\t", s.str());
}

TEST(LabelTests, PrintHieroReorder) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts[0] = {{0, 1}, {1, 2}};
  rule.nts[1] = {{1, 2}, {0, 1}};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X]\t[X,1] [X,2]\t[X,2] [X,1]\t", s.str());
}

TEST(LabelTests, PrintHieroMixed) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts[0] = {{0, 1}, {1, 2}};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X]\t[X,1] c\tb [X,1]\t", s.str());
}

TEST(LabelTests, PrintHieroMixed2) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d e\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts[0] = {{0, 1}, {0, 1}};
  rule.nts[1] = {{1, 2}, {2, 3}};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X]\t[X,1] [X,2]\t[X,1] d [X,2]\t", s.str());
}

TEST(LabelTests, SAMTConstituent) {
  auto t = readTree("(ROOT (A foo) (B bar))");
  SAMTLabeler sl(t);
  EXPECT_EQ("A", sl(SpanPair{{}, { 0, 1 }}));
}

}
