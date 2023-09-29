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

// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"

extern "C" {
//TSLanguage *tree_sitter_json();
TSLanguage *tree_sitter_socialgaming();
}



TEST(ParserTests, RPS_TEST) {
    // Create a language and parser.
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};

    // Parse the provided JSON string into a syntax tree.
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = parser.parseString(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();

    // GTest to see if correct number of children are read
    ASSERT_EQ(root.getNumChildren(), 19);
    std::cout << root.getNumChildren() << "\n";
    std::cout << root.getSExpr().get() << "\n";
    ts::Node n = root.getChild(0);

}
