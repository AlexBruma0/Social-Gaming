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

// Node classes
#include "ruleNode.h"

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
}

// verifys the print test method and checks the behaviour of games for 3 game types

TEST(ParserTests, PRINT_TEST_EMPTY_STRING){
    std::streambuf *original_cout = std::cout.rdbuf();
    std::stringstream buffer;
    std::cout.rdbuf(buffer.rdbuf());

    std::string noStringSourcecode = "";
    ts::Tree noStringTree = string_to_tree(noStringSourcecode);

    //get root nodes
    ts::Node noStringNode = noStringTree.getRootNode();

    //begin printNodeType testing
    std::string expectedNoStringNodePrint = "ERROR Children: 0\n";
    printNodeType(noStringNode);
    std::string noStringOutput = buffer.str();

    ASSERT_EQ(noStringOutput, expectedNoStringNodePrint);
    buffer.str("");

    //close output capture
    std::cout.rdbuf(original_cout);

}
TEST(ParserTests, PRINT_TEST_EMPTY_GAME){
    std::streambuf *original_cout = std::cout.rdbuf();
    std::stringstream buffer;
    std::cout.rdbuf(buffer.rdbuf());

    std::string emptySourcecode = file_to_string(RPS_LOCATION);
    ts::Tree emptyTree = string_to_tree(emptySourcecode);

    //get root nodes
    ts::Node emptyNode = emptyTree.getRootNode();

    //begin printNodeType testing
    std::string expectedEmptyPrint = "game Children: 18\n" ;
    printNodeType(emptyNode);
    std::string emptyOutput = buffer.str();

    ASSERT_EQ(emptyOutput, expectedEmptyPrint);
    buffer.str("");

    //close output capture
    std::cout.rdbuf(original_cout);
}
TEST(ParserTests, PRINT_TEST_RPS){
    std::streambuf *original_cout = std::cout.rdbuf();
    std::stringstream buffer;
    std::cout.rdbuf(buffer.rdbuf());

    std::string rpsSourcecode = file_to_string(EMPTY_GAME_LOCATION);
    ts::Tree rpsTree = string_to_tree(rpsSourcecode);

    //get root nodes
    ts::Node rpsNode = rpsTree.getRootNode();

    //begin printNodeType testing
    std::string expectedRPSNodePrint = "game Children: 6\n"; 
    printNodeType(rpsNode);
    std::string rpsOutput = buffer.str();

    ASSERT_EQ(rpsOutput, expectedRPSNodePrint);
    buffer.str("");

    //close output capture
    std::cout.rdbuf(original_cout);
}
std::string removeWhiteSpace(const std::string& input){
    std::string output{""};
    for(int i = 0; i < input.size(); i++){
        if(input[i] != ' '){
            output += input[i];

        }
    }
    return output;
}

// testing the removeLastNonSpaceBeforeClosingBracket() method

TEST(ParserTests, STRING_PARSING_UTILS_removeLastNonSpaceBeforeClosingBracket){
    //begin testing
    std::string parserInput = "configuration[  ,]";
    std::string parserInput2 = "per-player[]";
    std::string parserInput3 = "per-player[, ]";
    std::string expectedOutput = "configuration[  ]";
    std::string expectedOutput2 = "per-player[]";
    std::string expectedOutput3 = "per-player[ ]";

    std::string parserOutput = 
    removeLastNonSpaceBeforeClosingBracket(parserInput);
    std::string parserOutput2 =
    removeLastNonSpaceBeforeClosingBracket(parserInput2);
    std::string parserOutput3 = 
    removeLastNonSpaceBeforeClosingBracket(parserInput3);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);
}

// testing the replaceSubstring() method
TEST(ParserTests, STRING_PARSING_UTILS_replaceSubstring){
    std::string parserInput = "configuration[  ,]";
    std::string parserInput2 = "per-player[]";
    std::string parserInput3 = "per-player[, ]";

    std::string replacementSubstring = "configuration";
    std::string newSubstring = "per-player";
    std::string replacementSubstring2 = "per-player";
    std::string newSubstring2 = "configuration";
    std::string replacementSubstring3 = "per-player";
    std::string newSubstring3 = "";
    std::string expectedOutput = "per-player[  ,]";
    std::string expectedOutput2 = "configuration[]";
    std::string expectedOutput3 = "[, ]";

    std::string parserOutput = parserInput;
    replaceSubstring(parserOutput, replacementSubstring, newSubstring);
    std::string parserOutput2 = parserInput2;
    replaceSubstring(parserOutput2, replacementSubstring2, newSubstring2);
    std::string parserOutput3 = parserInput3;
    replaceSubstring(parserOutput3, replacementSubstring3, newSubstring3);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);

}

// testing the removeCharacterAtIndex() method
TEST(ParserTests, STRING_PARSING_UTILS_removeCharacterAtIndex){
    std::string parserInput = "configuration[  ,]";
    std::string parserInput2 = "per-player[]";
    std::string parserInput3 = "per-player[, ]";

    std::string expectedOutput = "onfiguration[  ,]";
    std::string expectedOutput2 = "perplayer";
    std::string expectedOutput3 = "per-player[, ]";

    std::string parserOutput = removeCharacterAtIndex(parserInput, 0);
    std::string parserOutput2 = removeCharacterAtIndex(parserInput2, 3);
    parserOutput2 = removeCharacterAtIndex(parserOutput2, parserOutput2.size()-1);
    parserOutput2 = removeCharacterAtIndex(parserOutput2, parserOutput2.size()-1);
    std::string parserOutput3 = removeCharacterAtIndex(parserInput3, 99);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);

}

// testing the deleteCommaInRegularExpression() method
TEST(ParserTests, STRING_PARSING_UTILS_deleteCommaInRegularExpression){
    std::string parserInput = "text,  }  text,  }";
    std::string parserInput2 = "},},},},}";
    std::string parserInput3 = "text, ";

    std::string expectedOutput = "text  }  text,  }";
    std::string expectedOutput2 = "}},},},}";
    std::string expectedOutput3 = "text, ";

    std::string parserOutput = parserInput;
    std::string parserOutput2 = parserInput2;
    std::string parserOutput3 = parserInput3;

    deleteCommaInRegularExpression(parserOutput);
    deleteCommaInRegularExpression(parserOutput2);
    deleteCommaInRegularExpression(parserOutput3);

    ASSERT_EQ(parserOutput, expectedOutput);
    ASSERT_EQ(parserOutput2, expectedOutput2);
    ASSERT_EQ(parserOutput3, expectedOutput3);


}
// testing the getNodeStringValue() method

TEST(ParserTests, STRING_PARSING_UTILS_getNodeStringValue){
    // tree setups
    std::string noStringSourcecode = "";
    ts::Tree noStringTree = string_to_tree(noStringSourcecode);

    ts::Node noStringNode = noStringTree.getRootNode();

    try{
        std::string nodeValOutput = getNodeStringValue(noStringNode, noStringSourcecode);
        ASSERT_EQ(1,0);
    }
    catch(std::exception e){
        ASSERT_EQ(0,0);
    }

}
// testing the formatString() method
TEST(ParserTests, STRING_PARSING_UTILS_formatString){
    std::string formatInput = "";
    std::string formatInput2 =

    "variables{"
    "   winners: []"
    "}";
    std::string formatInput3 =

    R"({
        configuration {
        name: "Rock, Paper, Scissors"
        player range: (2, 4)
        audience: false
        setup: {
            rounds {
            kind: integer
            prompt: "The number of rounds to play"
            range: (1, 20)
            }
        }
        }
    })";

    std::string formatExpected = "";
    std::string formatExpected2 = "\"variables\"{   \"winners\": []}";
    std::string formatExpected3 =
    R"({
        "configuration": {
            "name": "Rock, Paper, Scissors",
            "player_range": [2, 4],
            "audience": "false",
            "setup": {
                "rounds": {
                    "kind": "integer",
                    "prompt": "The number of rounds to play",
                    "range": [1, 20]
                }
            }
        }
    })";
    

    std::string formatOutput = formatString(formatInput);
    std::string formatOutput2 = formatString(formatInput2);
    std::string formatOutput3 = formatString(formatInput3);

    ASSERT_EQ(formatOutput, formatExpected);
    ASSERT_EQ(formatOutput2, formatExpected2);
    ASSERT_EQ(removeWhiteSpace(formatOutput3) , removeWhiteSpace(formatExpected3));
}

// testing the generateNumbersList() method
TEST(ParserTests, JSON_UTILS_TEST_generateNumbersList){

    json expectedOutput{0,1,2,3,4};
    int startRange = 0;
    int endRange = 4;

    json output = generateNumbersList(startRange, endRange);
    ASSERT_EQ(output, expectedOutput);

    json expectedOutput2{4};
    startRange = 4;
    endRange = 4;

    output = generateNumbersList(startRange, endRange);
    ASSERT_EQ(output, expectedOutput2);

    json expectedOutput3{};
    startRange = 99;
    endRange = -1;

    output = generateNumbersList(startRange, endRange);
    ASSERT_EQ(output, expectedOutput3);



}

// this test is for checking behaviour while building the rules tree
// we'll add mocks soon
TEST(ParserTests, OP_TREE_TEST) {
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    ts::Node root = tree.getRootNode();

    TreeNode node = buildRuleTree(root, sourcecode);

    //node.execute();
}