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

TEST(LabelTests, PrintHiero) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  PhrasalRule rule{asp};
  std::ostringstream s;
  s << LabeledRuleView{ rule, HieroLabeler{} };
  EXPECT_EQ("[X] ||| a ||| b", s.str());
}

}
