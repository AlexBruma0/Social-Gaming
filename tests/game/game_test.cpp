#include <gtest/gtest.h>
#include "Game.h"

// Demonstrate some basic assertions.
TEST(GameTests, Initial) {
  Game* game = new Game();
  assert(game->config.getAudience() == 0);
  assert(game->config.getName() == "undefined");
  assert(game->config.player_range == Range(0,0));
  assert(game->config.setup.deflt == 0);
  assert(game->config.setup.prompt == "undefined");
  assert(game->config.setup.kind == -1);
  assert(game->config.setup.range == Range(0, 0));
}
