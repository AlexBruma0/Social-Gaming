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

// helper methods
#include "utils.h"

// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#define EMPTY_GAME_LOCATION "resources/games/empty.game"

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
    ASSERT_EQ(root.getNumChildren(), 7);
    //std::cout << root.getNumChildren() << "\n";

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
    ASSERT_EQ(root.getNumChildren(), 19);
    //std::cout << root.getNumChildren() << "\n";

    // Printing the tree; leave commented out unless you want to see it
    //std::cout << root.getSExpr().get() << "\n";
    dfs(root, sourcecode);
}




// TEST(ParserTests, RPS_TEST_JSON) {
//     const int num_players = 3;
//     const int num_rounds = 2;

//     std::string sourcecode = file_to_string(RPS_LOCATION);
//     ts::Tree tree = string_to_tree(sourcecode);

//     // Access the root node of the AST
//     ts::Node root = tree.getRootNode();

//     json jsonData = create_json_data(root, sourcecode);

//     jsonData["configuration"]["rounds"] = num_rounds;
//     for(int i = 0; i < num_players; i++){
//         jsonData["players"][i] = jsonData["per-player"];
//     }

//     std::cout << jsonData.dump() << "\n";

// }

TEST(ParserTests, PRINT_TEST){
    //setup output capture
    std::streambuf *original_cout = std::cout.rdbuf();
    std::stringstream buffer;
    std::cout.rdbuf(buffer.rdbuf());

    //tree setups
    std::string noStringSourcecode = "";
    ts::Tree no_string_tree = string_to_tree(noStringSourcecode);
    std::string emptySourcecode = file_to_string(RPS_LOCATION);
    ts::Tree empty_tree = string_to_tree(emptySourcecode);
    std::string rpsSourcecode = file_to_string(EMPTY_GAME_LOCATION);
    ts::Tree rps_tree = string_to_tree(rpsSourcecode);

    //get root nodes
    ts::Node no_string_node = no_string_tree.getRootNode();
    ts::Node empty_node = empty_tree.getRootNode();
    ts::Node rps_node = rps_tree.getRootNode();

    //begin printNode testing
    std::string expectedNoStringNodePrint = "ERROR Children: 0\n";
    printNode(no_string_node);
    std::string noStringOutput = buffer.str();

    ASSERT_EQ(noStringOutput, expectedNoStringNodePrint);
    buffer.str("");

    std::string expectedEmptyPrint = "game Children: 19\n" ;
    printNode(empty_node);
    std::string emptyOutput = buffer.str();

    ASSERT_EQ(emptyOutput, expectedEmptyPrint);
    buffer.str("");

    std::string expectedRPSNodePrint = "game Children: 7\n"; 
    printNode(rps_node);
    std::string output = buffer.str();

    ASSERT_EQ(output, expectedRPSNodePrint);
    buffer.str("");

    //begin printNodeValue testing


    //close output capture
    std::cout.rdbuf(original_cout);
    // print_node_value(no_string_node, noStringSourcecode);
    //print_node_value(empty_node, emptySourcecode);
    //print_node_value(rps_node, rpsSourcecode);
}

TEST(ParserTests, PARSING_UTILS_TEST){
    // tree setups
    std::string noStringSourcecode = "";
    ts::Tree no_string_tree = string_to_tree(noStringSourcecode);
    std::string emptySourcecode = file_to_string(RPS_LOCATION);
    ts::Tree empty_tree = string_to_tree(emptySourcecode);
    std::string rpsSourcecode = file_to_string(EMPTY_GAME_LOCATION);
    ts::Tree rps_tree = string_to_tree(rpsSourcecode);

    // get root nodes
    ts::Node no_string_node = no_string_tree.getRootNode();
    ts::Node empty_node = empty_tree.getRootNode();
    ts::Node rps_node = rps_tree.getRootNode();

    //begin testing
    std::string parserInput = "configuration[  ,]";
    std::string parserInput2 = "per-player[]";
    std::string parserInput3 = "per-player[, ]";
    std::string expectedOutput = "configuration[  ]";
    std::string expectedOutput2 = "per-player[]";
    std::string expectedOutput3 = "per-player[ ]";

    std::string parserOutput = 
    RemoveLastNonSpaceBeforeClosingBracket(parserInput);
    std::string parserOutput2 =
    RemoveLastNonSpaceBeforeClosingBracket(parserInput2);
    std::string parserOutput3 = 
    RemoveLastNonSpaceBeforeClosingBracket(parserInput3);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);

    std::string replacementSubstring = "configuration";
    std::string newSubstring = "per-player";
    std::string replacementSubstring2 = "per-player";
    std::string newSubstring2 = "configuration";
    std::string replacementSubstring3 = "per-player";
    std::string newSubstring3 = "";
    expectedOutput = "per-player[  ,]";
    expectedOutput2 = "configuration[]";
    expectedOutput3 = "[, ]";

    parserOutput = parserInput;
    replaceSubstring(parserOutput, replacementSubstring, newSubstring);
    parserOutput2 = parserInput2;
    replaceSubstring(parserOutput2, replacementSubstring2, newSubstring2);
    parserOutput3 = parserInput3;
    replaceSubstring(parserOutput3, replacementSubstring3, newSubstring3);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);

    expectedOutput = "onfiguration[  ,]";
    expectedOutput2 = "perplayer";
    expectedOutput3 = "per-player[, ]";

    parserOutput = removeCharacterAtIndex(parserInput, 0);
    parserOutput2 = removeCharacterAtIndex(parserInput2, 3);
    parserOutput2 = removeCharacterAtIndex(parserOutput2, parserOutput2.size()-1);
    parserOutput2 = removeCharacterAtIndex(parserOutput2, parserOutput2.size()-1);
    parserOutput3 = removeCharacterAtIndex(parserInput3, 99);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);
}


