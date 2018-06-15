#include "tree.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

namespace {

void expectNode(
    IndexType s, IndexType e, const char*l, const Node* n, Node actual) {
  Span sp{ s, e };
  EXPECT_EQ(sp, actual.span);
  EXPECT_EQ(l, actual.label);
  EXPECT_EQ(n, actual.parent);
}

}

TEST(TreeTests, EmptyString) {
  auto t = readTree("");
  EXPECT_EQ(0, t.size());
}

TEST(TreeTests, OneNode) {
  auto t = readTree("(ROOT foo)");
  ASSERT_EQ(1, t.size());
  auto root = t.front();
  auto s = Span{0, 1};
  expectNode(0, 1, "ROOT", nullptr, root);
}

TEST(TreeTests, Small) {
  auto t = readTree("(ROOT (A foo) (B bar))");
  ASSERT_EQ(3, t.size());
  expectNode(0, 2, "ROOT", nullptr, t[0]);
  expectNode(0, 1, "A", &t[0], t[1]);
  expectNode(1, 2, "B", &t[0], t[2]);
}

TEST(TreeTests, TooFewRightParens) {
 EXPECT_THROW(readTree("(ROOT"), std::invalid_argument);
}

TEST(TreeTests, TooManyRightParens) {
 EXPECT_THROW(readTree("(ROOT foo))"), std::invalid_argument);
}

}
