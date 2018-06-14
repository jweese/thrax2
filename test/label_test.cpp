#include "label.h"
#include "phrasalrule.h"
#include "gtest/gtest.h"

#include <sstream>

namespace jhu::thrax {

TEST(LabelTests, Hiero) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  SpanPair sp;
  HieroLabeler hl;
  ASSERT_EQ("X", hl(asp, sp));
}

TEST(LabelTests, PrintHieroLexical) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  PhrasalRule rule{asp};
  std::ostringstream s;
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| a ||| b", s.str());
}

TEST(LabelTests, PrintHieroLexicalMulti) {
  auto asp = readAlignedSentencePair<false, false>("a\tb c d\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| a ||| b c d", s.str());
}

TEST(LabelTests, PrintHieroUnary) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts.front() = asp.span();
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| [X,1] ||| [X,1]", s.str());
}

TEST(LabelTests, PrintHieroReorder) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts[0] = {{0, 1}, {1, 2}};
  rule.nts[1] = {{1, 2}, {0, 1}};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| [X,1] [X,2] ||| [X,2] [X,1]", s.str());
}

TEST(LabelTests, PrintHieroMixed) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts[0] = {{0, 1}, {1, 2}};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| [X,1] c ||| b [X,1]", s.str());
}

TEST(LabelTests, PrintHieroMixed2) {
  auto asp = readAlignedSentencePair<false, false>("a c\tb d e\t0-0");
  std::ostringstream s;
  PhrasalRule rule{asp};
  rule.nts[0] = {{0, 1}, {0, 1}};
  rule.nts[1] = {{1, 2}, {2, 3}};
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| [X,1] [X,2] ||| [X,1] d [X,2]", s.str());
}

}