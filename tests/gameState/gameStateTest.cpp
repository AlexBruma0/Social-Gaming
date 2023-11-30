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
    GameState gs{&jsonData, nullptr};
    std::cout << gs.getState()->dump();

}

TEST(gameStateTest, GAMEVARIABLES_BASIC) {
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);
    ts::Node root = tree.getRootNode();
    json jsonData = createJsonData(root, sourcecode);
    GameVariables gv{};

    gv.insert("test1", std::string("test"));
    gv.insert("test2", 1);

    GameState gs{&jsonData, nullptr};
    gs.setVars(gv);

    auto result = std::get<std::string>(gs.getVars()->getValue("test1"));
    std::string expected = "test";

    ASSERT_EQ(result, expected);

    auto result2 = std::get<int>(gs.getVars()->getValue("test2"));
    int expected2 = 1;

    ASSERT_EQ(result2, expected2);
}

TEST(gameStateTest, GAMEVARIABLES_ARRAYS) {
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);
    ts::Node root = tree.getRootNode();
    json jsonData = createJsonData(root, sourcecode);
    GameVariables gv{};

    gv.insert("test1", std::vector<int>{1, 2, 3});
    gv.insert("test2", std::vector<std::string>{"1", "2", "3"});

    GameState gs{&jsonData, nullptr};
    gs.setVars(gv);

    auto result = std::get<std::vector<int>>(gs.getVars()->getValue("test1"));
    auto expected = std::vector<int>{1, 2, 3};
    ASSERT_EQ(result, expected);

    auto result2 = std::get<std::vector<std::string>>(gs.getVars()->getValue("test2"));
    auto expected2 = std::vector<std::string>{"1", "2", "3"};
    ASSERT_EQ(result, expected);
}

TEST(gameStateTest, GAMEVARIABLES_NESTED) {
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);
    ts::Node root = tree.getRootNode();
    json jsonData = createJsonData(root, sourcecode);
    GameVariables gv{};
    GameVariables gv2{};
    GameVariables gv3{};

    gv3.insert("third", 3);
    gv2.insert("second", gv3);
    gv.insert("first", gv2);

    GameState gs{&jsonData, nullptr};
    gs.setVars(gv);

    ASSERT_TRUE(std::holds_alternative<GameVariables>(gs.getVars()->getValue("first")));
    ASSERT_TRUE(std::holds_alternative<GameVariables>(std::get<GameVariables>(gs.getVars()->getValue("first")).getValue("second")));

    // note: you shouldn't actually get nested things like this, it's just to make sure it works
    auto result = std::get<int>(std::get<GameVariables>(std::get<GameVariables>(gs.getVars()->getValue("first")).getValue("second")).getValue("third"));
    int expected = 3;

    ASSERT_EQ(result, expected);

    // this is the preferred way to get nested variables
    auto result2 = gs.getVars()->getNestedMap("first.second.third");
    int expected2 = 3;

    ASSERT_EQ(result, expected);
}

TEST(gameStateTest, GAMEVARIABLES_ERASE) {
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);
    ts::Node root = tree.getRootNode();
    json jsonData = createJsonData(root, sourcecode);
    GameVariables gv{};

    gv.insert("deleted", 1);
    gv.insert("not deleted", std::string("yay!"));
    gv.erase("deleted");

    GameState gs{&jsonData, nullptr};
    gs.setVars(gv);

    auto result = gs.getVars()->getValue("deleted");

    // the result should not hold any valid type, which means that using visit on standard types will ignore it
    ASSERT_FALSE(std::holds_alternative<int>(result));
    ASSERT_FALSE(std::holds_alternative<std::string>(result));
    ASSERT_FALSE(std::holds_alternative<std::vector<int>>(result));
    ASSERT_FALSE(std::holds_alternative<std::vector<std::string>>(result));
    ASSERT_FALSE(std::holds_alternative<GameVariables>(result));
    ASSERT_FALSE(std::holds_alternative<std::vector<GameVariables>>(result));

    // make sure that it returns the dummy type
    ASSERT_TRUE(std::holds_alternative<NotFoundTag>(result));

    auto result2 = std::get<std::string>(gs.getVars()->getValue("not deleted"));
    std::string expected2 = "yay!";

    ASSERT_EQ(result2, expected2);

    // check that erasing something that doesn't exist causes no issues
    gv.erase("blah");

    ASSERT_TRUE(true);
}