#include "tree.h"
#include "gtest/gtest.h"

namespace jhu::thrax {

TEST(TreeTests, EmptyString) {
  auto t = readTree("");
  EXPECT_EQ(0, t.size());
}

}
