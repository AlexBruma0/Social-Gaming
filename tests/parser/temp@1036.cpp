#include <gtest/gtest.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <span>

// Wrapper API
#include <cpp-tree-sitter.h>

// Custom Library in lib
#include "parser.h"
#include <variant>
// Game Parser functionality
#include "gameParser.h"

// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#define EMPTY_GAME_LOCATION "resources/games/empty.game"

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}
struct Configuration {
    int rounds;  // Assume rounds is an integer
    // Add other configuration properties as needed
};



// Helper function to generate a list of numbers from 1 to count
std::vector<int> generateNumbersList(int start, int end) {
    std::vector<int> numbers;
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}


// Implement other parsing methods for each statement type
// Add similar methods for other statement types
void printNode (const ts::Node& node){
std::cout << "Node Type: " << node.getType() << "              Children: " << node.getNumChildren() << std::endl;
}

void print_bfs(const ts::Node& node){
    for(int i = 0; i < node.getNumChildren(); i++){
        printNode(node.getChild(i));
    }
}

void executeStatementList(const ts::Node &sub_tree_node_to_execute, const std::string& source_code) {

    if(sub_tree_node_to_execute.getType() == "rule"){

        const ts::Node exeuctable_node = sub_tree_node_to_execute.getChild(0);
        if(exeuctable_node.getType() == "for" || exeuctable_node.getType() == "parallel_for"){

            const ts::Node node = exeuctable_node.getChild(1);
            std::string variable = getSubstringByByteRange(source_code, node.getByteRange().start, node.getByteRange().end );
            std::cout << "variable: " << variable << std::endl;

            const ts::Node list_node = exeuctable_node.getChild(3);
            if(!list_node.getChild(2).isNull()){
                const ts::Node ex = list_node.getChild(0).getChild(0);
                std::string variable = getSubstringByByteRange(source_code, ex.getByteRange().start, ex.getByteRange().end );
                std::cout << "expresion: " << variable << std::endl;
                std::cout << "\n";
            }
            print_bfs(list_node);
            std::cout << "\n";


        }
    }

    //Recursively visit children nodes
    for (uint32_t i = 0; i < sub_tree_node_to_execute.getNumChildren(); ++i) {
        executeStatementList(sub_tree_node_to_execute.getChild(i), source_code);
    }
}


// Function to interpret the "for" construct with a generic list type
template<typename T>
void executeForEachLoop(const std::string &variable, const T &list, const ts::Node& sub_tree_node_to_execute) {
    // Traverse the list of elements and execute the statement list for each element
    for (const auto &element : list) {
        std::cout << "Executing for " << variable << " = " << element << ":\n";

        // Execute the statement list for the current element
        //executeStatementList(sub_tree_node_to_execute);

        // For demonstration, we'll just print the element
        std::cout << "Statement executed for " << variable << " = " << element << "\n";
    }
}




// Overloaded version of interpretFor to handle numerical attributes
void executeForEachLoop(const std::string &variable, const int& start, const int& end, const ts::Node& rules_node) {
    // Extract the count from the expression
    std::vector<int> numbersList = generateNumbersList(start, end);
    executeForEachLoop(variable, numbersList, rules_node);
        
}

TEST(ParserTests, RPS_TEST) {
    // Read file as SocialGaming code and parse into a syntax tree
    // Rules node = 17 
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();
    std::string_view s1{"rules"};
    std::string s2{"round"};
    // print_bfs(root);
    // std::cout << root.getFieldNameForChild(6) << "\n";
    // std::cout << root.getNumNamedChildren() << "\n";
    // std::cout << root.getNumChildren() << "\n";
    // std::cout << root.getChildByFieldName("comment").isNull() << "\n";
    // std::cout << root.getChildByFieldName("configuration").isNull() << "\n";
    // std::cout << root.getChildByFieldName("constants").isNull() << "\n";
    // std::cout << root.getChildByFieldName("variables").isNull() << "\n";
    // std::cout << root.getChildByFieldName("per_player").isNull() << "\n";
    // std::cout << root.getChildByFieldName("per_audience").isNull() << "\n";
    // std::cout << root.getChildByFieldName("rules").isNull() << "\n";
    // std::cout << root.getChildByFieldName("ERROR").isNull() << "\n";
    ts::Node rules_node = root.getChildByFieldName("rules");
    // print_bfs(rules_node);
    // std::cout << rules_node.getNumNamedChildren() << "\n";
    // std::cout << rules_node.getNumChildren() << "\n";
    // std::cout << rules_node.getChildByFieldName("rules").isNull() << "\n";
    // std::cout << rules_node.getChildByFieldName("body").isNull() << "\n";
    ts::Node rules_node_body = rules_node.getChildByFieldName("body");
    // print_bfs(rules_node_body);
    // std::cout << rules_node_body.getNumNamedChildren() << "\n";
    // std::cout << rules_node_body.getNumChildren() << "\n";
    // std::cout << rules_node.getChildByFieldName("{").isNull() << "\n";
    // std::cout << rules_node.getChildByFieldName("rule").isNull() << "\n";
    // std::cout << rules_node.getChildByFieldName("comment").isNull() << "\n";
    // std::cout << rules_node.getChildByFieldName("rule").isNull() << "\n";
    // std::cout << rules_node.getChildByFieldName("}").isNull() << "\n";
    // ts::Node rules_node_body_rule = rules_node_body.getChild(1);
    // print_bfs(rules_node_body_rule);
    // std::cout << rules_node_body_rule.getChildByFieldName("for").isNull() << "\n";
    // std::cout << rules_node_body.getChildByFieldName("rule").isNull() << "\n";
    // std::cout << rules_node_body.getChildByFieldName("comment").isNull() << "\n";
    // std::cout << rules_node_body.getChildByFieldName("}").isNull() << "\n";
    //std::cout << rules_node_body.getChildByFieldName("rule").isNull() << "\n";
    //print_bfs(rules_node_body);
    //executeForEachLoop(s2, 1, 2, rules_node_body);
    //dfs(rules_node_body, sourcecode);
    executeStatementList(rules_node_body, sourcecode);
    //rules.body.firstrule
    //bfs(rules_node_body);

}
