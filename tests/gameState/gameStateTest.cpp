#include <gtest/gtest.h>
#include "GameState.h"

// Wrapper API
#include <cpp-tree-sitter.h>

// Game Parser functionality
#include "gameParser.h"

// Custom Library in lib
#include "parser.h"

// helper methods
#include "utils.h"

// Current Directory is your build directory
#ifndef RPS_LOCATION
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#endif

#ifndef EMPTY_GAME_LOCATION
#define EMPTY_GAME_LOCATION "resources/games/empty.game"
#endif

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

// Demonstrate some basic assertions.
TEST(gameStateTest, Initial) {
    const int num_players = 2;
    const int num_rounds = 2;

    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();


    json jsonData = createJsonData(root, sourcecode);

    jsonData["configuration"]["rounds"] = num_rounds;
    for(int i = 0; i < num_players; i++){
        jsonData["players"][i] = jsonData["per-player"];
    }
    GameState gs{&jsonData};
    std::cout << gs.getState()->dump();

}