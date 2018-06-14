#include "label.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(LabelTests, Hiero) {
  auto asp = readAlignedSentencePair<false, false>("a\tb\t0-0");
  SpanPair sp;
  HieroLabeler hl;
  ASSERT_EQ("X", hl(asp, sp));
}

}
