#include "sentence.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(SentenceTests, EmptyString) {
  std::string_view line = "";
  auto s = tokens<false>(line);
  EXPECT_EQ(0, s.size());
}

TEST(SentenceTests, Whitespace) {
  std::string_view line = "       ";
  auto s = tokens<false>(line);
  EXPECT_EQ(0, s.size());
}

TEST(SentenceTests, Trim) {
  std::string_view line = "  foo     ";
  auto s = tokens<false>(line);
  ASSERT_EQ(1, s.size());
  EXPECT_EQ("foo", s.front());
}

TEST(SentenceTests, ManyTokens) {
  std::string_view line = "  foo bar  baz    ";
  auto s = tokens<false>(line);
  ASSERT_EQ(3, s.size());
  EXPECT_EQ("foo", s[0]);
  EXPECT_EQ("bar", s[1]);
  EXPECT_EQ("baz", s[2]);
}

TEST(SentenceTests, EmptyParse) {
  std::string_view line = "  ()     ";
  auto s = tokens<true>(line);
  EXPECT_EQ(0, s.size());
}

TEST(SentenceTests, SimpleParse) {
  std::string_view line = "(FOO bar)";
  auto s = tokens<true>(line);
  ASSERT_EQ(1, s.size());
  EXPECT_EQ("bar", s.front());
}

TEST(SentenceTests, LargerParse) {
  std::string_view line = "(FOO (BAR baz) (QUX blah))";
  auto s = tokens<true>(line);
  ASSERT_EQ(2, s.size());
  EXPECT_EQ("baz", s[0]);
  EXPECT_EQ("blah", s[1]);
}

TEST(SentenceTests, InconsistentAlignedSentencePair) {
  std::string_view line = "foo bar baz";
  auto s = tokens<false>(line);
  auto a = readAlignment("0-0 1-3");
  EXPECT_THROW(AlignedSentencePair(s, s, a), std::out_of_range);
}

TEST(SentenceTests, ReadAlignedSentencePair) {
  std::string_view line = "foo bar baz\tqux\t0-0 1-0 2-0";
  auto pair = readAlignedSentencePair<false, false>(line);
  ASSERT_EQ(3, pair.src.size());
  ASSERT_EQ(1, pair.tgt.size());
  ASSERT_EQ(3, pair.alignment.size());
}

}
