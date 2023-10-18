#include <gtest/gtest.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>

// Wrapper API
#include <cpp-tree-sitter.h>

// Custom Library in lib
#include "parser.h"

// Game Parser functionality
#include "gameParser.h"

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

TEST (ParserTests, NO_SOURCECODE_TEST) {
    std::string sourcecode = "";
    ts::Tree tree = string_to_tree(sourcecode);

    ts::Node root = tree.getRootNode();
    ASSERT_FALSE(root.isNull());
    ASSERT_EQ(root.getNumChildren(), 0);
    ASSERT_EQ(root.getType(), "ERROR");

}

TEST(ParserTests, EMPTY_TEST) {
    // Read file as SocialGaming code and parse into a syntax tree
    std::string sourcecode = file_to_string(EMPTY_GAME_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();

    // GTest to see if correct number of children are read
    ASSERT_EQ(root.getNumChildren(), 6);
    std::cout << root.getNumChildren() << "\n";

    ASSERT_EQ(root.getType(), "game");
    ASSERT_EQ(root.getChild(0).getType(), "configuration");
    ASSERT_EQ(root.getChild(1).getType(), "constants");
    ASSERT_EQ(root.getChild(2).getType(), "variables");
    ASSERT_EQ(root.getChild(3).getType(), "per_player");
    ASSERT_EQ(root.getChild(4).getType(), "per_audience");
    ASSERT_EQ(root.getChild(5).getType(), "rules");

    // Printing the tree; leave commented out unless you want to see it
    //std::cout << root.getSExpr().get() << "\n";
    dfs(root, sourcecode);
}

TEST(ParserTests, RPS_TEST) {
    // Read file as SocialGaming code and parse into a syntax tree
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();

    // GTest to see if correct number of children are read
    ASSERT_EQ(root.getNumChildren(), 18);
    std::cout << root.getNumChildren() << "\n";

    // Printing the tree; leave commented out unless you want to see it
    //std::cout << root.getSExpr().get() << "\n";
    dfs(root, sourcecode);
}
