#include "optional.hpp"

#include <gtest/gtest.h>

using namespace palkin;

TEST(FundamentalType, CtorTest) {
  Optional<int> value(15);
  const auto v = value.get();
  ASSERT_EQ(15, v);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}