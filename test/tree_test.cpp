#include "tree.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(TreeTests, EmptyString) {
  auto t = readTree("");
  EXPECT_EQ(0, t.size());
}

TEST(TreeTests, OneNode) {
  auto t = readTree("(ROOT foo)");
  ASSERT_EQ(1, t.size());
  auto root = t.front();
  auto s = Span{0, 1};
  EXPECT_EQ(s, root.span);
  EXPECT_EQ("ROOT", root.label);
  EXPECT_EQ(nullptr, root.parent);
}

}
