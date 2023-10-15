#include <gtest/gtest.h>
#include "Game.h"

// Demonstrate some basic assertions.
TEST(GameTests, Initial) {
  std::string s = "TestGameState";
  Game *game = new Game(s);
  assert(game->getString() == s);
  std::string newS = "GameStateTest";
  game->setString(newS);
  assert(game->getString() == newS);
}
