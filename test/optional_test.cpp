#include "optional.hpp"

#include <gtest/gtest.h>

using namespace palkin;

TEST(FundamentalType, CtorTest) {
  Optional<int> value(42);
  const auto v = value.value();
  ASSERT_EQ(v, 42);
  // TODO
}

struct DeletedCopyConstruct {
  DeletedCopyConstruct() = default;
  DeletedCopyConstruct(int val) : value(val) {}
  DeletedCopyConstruct(const DeletedCopyConstruct&) = delete;
  DeletedCopyConstruct(DeletedCopyConstruct&&) = default;
  DeletedCopyConstruct& operator=(const DeletedCopyConstruct&) = default;
  DeletedCopyConstruct& operator=(DeletedCopyConstruct&&) = default;

  int value;
};

TEST(Ctors, DeletedCopyCtor) {
  Optional<DeletedCopyConstruct> value{42};
  // TODO
}

TEST(Ctors, CopyCtor) {
  Optional<int> value(42);
  Optional<int> value2(value);
  ASSERT_EQ(value2.value(), 42);
  // TODO
}

TEST(Ctors, MoveCtor) {
  Optional<int> value(42);
  Optional<int> value2(24);
  // value = std::move(value2);
  // ASSERT_EQ(value, 24);
  // TODO
}

TEST(Ctors, CopyAssign) {
  Optional<int> value(42);
  Optional<int> value2(24);
  value = value2;
  // TODO
}

TEST(Ctors, MoveAssign) {
  Optional<int> value(42);
  Optional<int> value2(24);
  value = std::move(value2);
  // TODO
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}