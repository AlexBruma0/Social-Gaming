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
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#define EMPTY_GAME_LOCATION "resources/games/empty.game"

extern "C" {
//TSLanguage *tree_sitter_json();
TSLanguage *tree_sitter_socialgaming();
}

TEST(ParserTests, RPS_TEST) {
    // Read file as SocialGaming code and parse into a syntax tree
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();

    // GTest to see if correct number of children are read
    ASSERT_EQ(root.getNumChildren(), 19);
    std::cout << root.getNumChildren() << "\n";
    //std::cout << root.getSExpr().get() << "\n";
    dfs(root);
    ts::Node n = root.getChild(0);
}

TEST(ParserTests, EMPTY_TEST) {
    // Read file as SocialGaming code and parse into a syntax tree
    std::string sourcecode = file_to_string(EMPTY_GAME_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();

    // GTest to see if correct number of children are read
    ASSERT_EQ(root.getNumChildren(), 7);
    std::cout << root.getNumChildren() << "\n";
    //std::cout << root.getSExpr().get() << "\n";
    dfs(root);
    ts::Node n = root.getChild(0);
}