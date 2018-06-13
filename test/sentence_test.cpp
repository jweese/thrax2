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

}
