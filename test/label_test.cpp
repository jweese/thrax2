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
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("A", sl(SpanPair{{}, { 0, 1 }}));
}

TEST(LabelTests, SAMTForwardApply) {
  auto t = readTree("(ROOT foo (B bar))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("ROOT/B", sl(SpanPair{{}, { 0, 1 }}));
}

TEST(LabelTests, SAMTBackwardApply) {
  auto t = readTree("(ROOT (A foo) bar)");
  ASSERT_EQ(2, t.size());
  Span root{0, 2};
  EXPECT_EQ(root, t.front().span);
  root.end = 1;
  EXPECT_EQ(root, t.back().span);
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("ROOT\\A", sl(SpanPair{{}, { 1, 2 }}));
}

TEST(LabelTests, SAMTConcat) {
  auto t = readTree("(ROOT baz (A foo) (B bar))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("A+B", sl(SpanPair{{}, { 1, 3 }}));
}

TEST(LabelTests, SAMTDoubleConcat) {
  auto t = readTree("(ROOT baz (A foo) (B bar) (C qux))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("A+B+C", sl(SpanPair{{}, { 1, 4 }}));
}

TEST(LabelTests, SAMTComma) {
  auto t = readTree("(ROOT baz (, foo) (B bar) (C qux))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("COMMA", sl(SpanPair{{}, { 1, 2 }}));
}

TEST(LabelTests, SAMTForwardApplyComma) {
  auto t = readTree("(ROOT foo (, bar))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("ROOT/COMMA", sl(SpanPair{{}, { 0, 1 }}));
}

TEST(LabelTests, SAMTBackwardApplyComma) {
  auto t = readTree("(ROOT (, foo) bar)");
  ASSERT_EQ(2, t.size());
  Span root{0, 2};
  EXPECT_EQ(root, t.front().span);
  root.end = 1;
  EXPECT_EQ(root, t.back().span);
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("ROOT\\COMMA", sl(SpanPair{{}, { 1, 2 }}));
}

TEST(LabelTests, SAMTConcatComma) {
  auto t = readTree("(ROOT baz (, foo) (B bar))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("COMMA+B", sl(SpanPair{{}, { 1, 3 }}));
}

TEST(LabelTests, SAMTDoubleConcatComma) {
  auto t = readTree("(ROOT baz (A foo) (, bar) (C qux))");
  SAMTLabeler sl(std::move(t));
  EXPECT_EQ("A+COMMA+C", sl(SpanPair{{}, { 1, 4 }}));
}

}
