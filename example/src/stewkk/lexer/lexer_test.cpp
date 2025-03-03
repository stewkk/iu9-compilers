#include <gmock/gmock.h>

using ::testing::Eq;

namespace stewkk::ipc {

TEST(ExampleTest, Example) {
  ASSERT_THAT(2+2, Eq(4));
}

}  // namespace stewkk::ipc
