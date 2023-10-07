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
struct AssignmentStatement {
    std::string variable;
    std::string value;
};

struct MessageStatement {
    std::string message;
};

struct ScoresStatement {
    std::vector<std::string> keys;
};

struct ExtendStatement {
    std::string namedList;
    std::vector<std::string> listExpression;
};

struct InputChoiceStatement {
    std::string player;
    std::string prompt;
    std::vector<std::string> choices;
    std::string target;
    int timeout;
};

struct ForEachStatement {
    std::string variable;
    std::string listExpression;
};

struct DiscardStatement {
    int count;
    std::string namedList;
};

struct MatchStatement {
    std::string expression;
    std::vector<std::pair<std::string, std::string>> cases; // Pair of expression and statement list
};

struct ExecutableStatement {
    // Define different types of statements in the game language
    enum class StatementType {
        Assignment,
        Message,
        Extend,
        InputChoice,
        ForEach,
        Discard,
        Match,
        Scores

    };

    // Define a variant to hold different types of statements
    std::variant<AssignmentStatement, MessageStatement, 
                                       ScoresStatement, ExtendStatement, 
                                       InputChoiceStatement, ForEachStatement, 
                                       DiscardStatement, MatchStatement, ScoresStatement> statement;

    StatementType type;
};
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

void executeScores(const ScoresStatement &statement) {
    // TODO: Implement execution logic for scores statement
    // This statement type handles printing scores based on keys
}

void executeExtend(const ExtendStatement &statement) {
    // TODO: Implement execution logic for extend statement
    // This statement type handles extending a named list with a list expression
}

void executeInputChoice(const InputChoiceStatement &statement) {
    // TODO: Implement execution logic for input choice statement
    // This statement type handles taking input choice from a player
}

void executeForEach(const ForEachStatement &statement) {
    // TODO: Implement execution logic for for-each statement
    // This statement type handles iterating over a list and executing a statement list
}

void executeDiscard(const DiscardStatement &statement) {
    // TODO: Implement execution logic for discard statement
    // This statement type handles discarding elements from a named list
}

void executeMatch(const MatchStatement &statement) {
    // TODO: Implement execution logic for match statement
    // This statement type handles matching an expression with cases and executing the corresponding statement list
}



void executeStatement(const ExecutableStatement &statement) {
    // Dispatch based on the statement type
    switch (statement.type) {
        case ExecutableStatement::StatementType::Assignment:
            executeAssignmentStatement(std::get<AssignmentStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::Message:
            executeMessageStatement(std::get<MessageStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::Scores:
            executeScores(std::get<ScoresStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::Extend:
            executeExtend(std::get<ExtendStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::InputChoice:
            executeInputChoice(std::get<InputChoiceStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::ForEach:
            executeForEach(std::get<ForEachStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::Discard:
            executeDiscard(std::get<DiscardStatement>(statement.statement));
            break;
        case ExecutableStatement::StatementType::Match:
            executeMatch(std::get<MatchStatement>(statement.statement));
            break;
    }
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
ExecutableStatement parseStatement(const ts::Node &node) {
    // Assuming the node represents a statement
    ts::Node statementTypeNode = node.getChild(0);
    auto statementType = statementTypeNode.getType();

    // Create an ExecutableStatement based on the statement type
    ExecutableStatement executableStatement;
    if (statementType == "assignment_statement") {
        // Parse the assignment statement and set it in executableStatement
        executableStatement.type = ExecutableStatement::StatementType::Assignment;
        executableStatement.statement = parseAssignmentStatement(statementTypeNode);
    } else if (statementType == "message_statement") {
        // Parse the message statement and set it in executableStatement
        executableStatement.type = ExecutableStatement::StatementType::Message;
        executableStatement.statement = parseMessageStatement(statementTypeNode);
    } else if (statementType == "scores_statement") {
        // Parse the scores statement and set it in executableStatement
        executableStatement.type = ExecutableStatement::StatementType::Scores;
        //executableStatement.statement = parseScoresStatement(statementTypeNode);
    } else if (statementType == "extend_statement") {
        // Parse the extend statement and set it in executableStatement
        executableStatement.type = ExecutableStatement::StatementType::Extend;
        executableStatement.statement = parseExtendStatement(statementTypeNode);
    }
    // Add cases for other statement types as needed

    return executableStatement;
}

AssignmentStatement parseAssignmentStatement(const ts::Node &node) {
    // TODO: Implement parsing logic for assignment statement
}

MessageStatement parseMessageStatement(const ts::Node &node) {
    // TODO: Implement parsing logic for message statement
}

ScoresStatement parseScoresStatement(const ts::Node &node) {
    // TODO: Implement parsing logic for scores statement
}

ExtendStatement parseExtendStatement(const ts::Node &node) {
    // TODO: Implement parsing logic for extend statement
}

// Implement other parsing methods for each statement type
// Add similar methods for other statement types

void executeStatementList(ts::Node sub_tree_node_to_execute) {
    // Create a cursor for traversing the syntax tree
    ts::Cursor cursor = sub_tree_node_to_execute.getCursor();


    // Traverse the syntax tree
    while (cursor.gotoFirstChild()) {
        // Get the current node
        ts::Node current_node = cursor.getCurrentNode();

        // Check if the node represents a statement
        if (current_node.getType() == "rule") {
            // Execute the statement based on its type
            ts::Node executable_node = current_node.getChild(0);
            //TODO: create parseStatement() method
            ExecutableStatement executable_statement = parseStatement(executable_node); // Assuming you have a function to parse the statement
            executeStatement(executable_statement); // Assuming you have a function to execute a statement
        }

        // Recur for any child nodes
        executeStatementList(current_node);
        
        // Move to the next sibling node
        if (!cursor.gotoNextSibling()){
            return;
        };
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
    ts::Node rules_node = root.getChildByFieldName("rules");
    ts::Node rules_node_body = rules_node.getChildByFieldName("body");

    //executeForEachLoop(s2, 1, 2, rules_node_body);
    dfs(rules_node_body, sourcecode);
    //rules.body.firstrule
    //bfs(rules_node_body);

}
