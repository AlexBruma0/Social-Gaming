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
struct Statement {
    // Define different types of statements in the game language
    enum class StatementType {
        Assignment,
        Message,
        // Add other statement types as needed
    };

    // Define a variant to hold different types of statements
    std::variant<AssignmentStatement, MessageStatement> statement;

    StatementType type;
};

struct AssignmentStatement {
    std::string variable;
    std::string value;
};

struct MessageStatement {
    std::string message;
};

void executeStatement(const Statement &statement) {
    // Dispatch based on the statement type
    switch (statement.type) {
        case Statement::StatementType::Assignment:
            executeAssignmentStatement(std::get<AssignmentStatement>(statement.statement));
            break;
        case Statement::StatementType::Message:
            executeMessageStatement(std::get<MessageStatement>(statement.statement));
            break;
        // Add cases for other statement types as needed
    }
}

void executeAssignmentStatement(const AssignmentStatement &assignment) {
    // TODO: Execute the assignment statement
    // Example: Assign the value to the variable
    std::cout << "Assigning " << assignment.value << " to " << assignment.variable << "\n";
}

void executeMessageStatement(const MessageStatement &message) {
    // TODO: Execute the message statement
    // Example: Print the message
    std::cout << "Displaying message: " << message.message << "\n";
}

void printNode (const ts::Node& node){
std::cout << "Node Type: " << node.getType() << "              Children: " << node.getNumChildren() << std::endl;
}

void print_bfs(const ts::Node& node){
    for(int i = 0; i < node.getNumChildren(); i++){
        printNode(node.getChild(i));
    }
}
// Define data structures to store game state
struct Player {
    std::string name;
    std::string weaponChoice;
    int wins;
};

std::vector<Player> players;  // Store player state

std::unordered_map<std::string, std::vector<std::string>> constants;  // Store constants

// Helper function to generate a list of numbers from 1 to count
std::vector<int> generateNumbersList(int start, int end) {
    std::vector<int> numbers;
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}

// Function to interpret the "for" construct with a generic list type
template<typename T>
void interpretFor(const std::string &variable, const T &list, const ts::Node& sub_tree_node_to_execute) {
    // Traverse the list of elements and execute the statement list for each element
    for (const auto &element : list) {
        std::cout << "Executing for " << variable << " = " << element << ":\n";

        // TODO: Execute the statement list for the current element
        // You can call a function to handle the logic for the statement list
        for(int i = 0: i < sub_tree_node_to_execute.getNumChildren(); i++){
            executeStatement
        }

        // For demonstration, we'll just print the element
        std::cout << "Statement executed for " << variable << " = " << element << "\n";
    }
}

// Overloaded version of interpretFor to handle numerical attributes
void interpretFor(const std::string &variable, const int& start, const int& end, const ts::Node& rules_node) {
    // Extract the count from the expression
    std::vector<int> numbersList = generateNumbersList(start, end);
    interpretFor(variable, numbersList, rules_node);
        
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
    ts::Node rules_node = root.getChildByFieldName("rules");
    ts::Node rules_node_body = rules_node.getChildByFieldName("body");

    interpretFor(s2, 1, 2, rules_node_body);
    //rules.body.firstrule
    //bfs(rules_node_body);

}
