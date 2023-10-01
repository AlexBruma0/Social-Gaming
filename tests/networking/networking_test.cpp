#include <gtest/gtest.h>
#include "Client.h"

// Demonstrate some basic assertions.
TEST(NetworkingTests, Initial) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");

  // Testing you can access the client from the networking library
  networking::Client client{"localhost", "8000"};
}
