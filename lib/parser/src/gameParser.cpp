
#include "parser.h"
#include "utils.h"
#include <iostream>
#include <cstdio>
#include <cpp-tree-sitter.h>
#include <vector>
#include <exception>
#include <algorithm>
#include "ruleNode.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "GameState.h"



// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"

extern "C" {
    //TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

std::string file_to_string(const std::string path) {
    std::ifstream infile{path};
    if (!infile) {
        printf("Could not open file\n");
        return {};
    }
    return std::string{std::istreambuf_iterator<char>(infile), {}};
}

ts::Tree string_to_tree(const std::string tree_string) {
    // Create a language and parser
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};

    // Parse the provided serialized string into a syntax tree and return
    return parser.parseString(tree_string);
}

std::string getSubstringUsingByteRange(const std::string& input, size_t startByte, size_t endByte) {
    // Ensure startByte is within range
    if (startByte >= input.size())
        throw std::runtime_error("Start byte exceeds source code size, invalid range");

    // Adjust endByte if it's beyond the string length
    endByte = std::min(endByte, input.size() - 1);
    // Calculate the character indices for the substring
    size_t startIndex = startByte;
    size_t length = endByte - startByte;

    // Extract the substring
    return input.substr(startIndex, length);
}

std::string getSubstringForNode(const ts::Node& node, const std::string& source_code) {
    return getSubstringUsingByteRange(source_code, node.getByteRange().start, node.getByteRange().end);
}

void printIndents(int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << " ";
    }
}

// for debugging
void printDfs(const ts::Node& node, const std::string& source_code, int depth) {
// Skip nodes with type "comment"
    if (node.getType() == "comment") {
        return;
    }

    printIndents(depth);
    std::cout << "Depth: " << depth << ", Node Type: " << node.getType() << std::endl;
    printIndents(depth);
    std::cout << "substring: "
        << getSubstringUsingByteRange(source_code, node.getByteRange().start, node.getByteRange().end)
        << std::endl;

    // Recursively visit children nodes for all nodes
    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        printDfs(node.getChild(i), source_code, depth + 1);
    }
}


void identifyOperations(const ts::Node& node, const std::string& source_code, TreeNode& parentNode, GameState* gameState) {
    // list of stuff we need to implement, operation nodes defined by the language
    std::vector<std::string_view> allowedTypes = {
            "for", "loop", "parallel_for", "in_parallel", "match", "match_entry", "extend", "reverse", "shuffle",
            "sort", "deal", "discard", "assignment", "timer", "input_choice", "input_text", "input_vote",
            "input_range", "message", "scores"
    };

    std::string nodeType = std::string(node.getType());

    auto it = std::find(allowedTypes.begin(), allowedTypes.end(), nodeType);
    if (it != allowedTypes.end() && node.getNumChildren() > 0) {
        std::unique_ptr<TreeNode> child = std::make_unique<TreeNode>(node, nodeType, source_code, gameState);

        TreeNode& childRef = *child;
        parentNode.addChild(std::move(child));


        for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
            identifyOperations(node.getChild(i), source_code, childRef, gameState);
        }
    } else {
        for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
            identifyOperations(node.getChild(i), source_code, parentNode, gameState);
        }
    }

}

TreeNode buildRuleTree(const ts::Node& syntaxTree, const std::string& source_code) {
    const int num_players = 2;
    const int num_rounds = 2;

    ts::Tree tree = string_to_tree(source_code);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();


    json jsonData = createJsonData(root, source_code);

    jsonData["configuration"]["rounds"] = num_rounds;
    jsonData["players"][0] = jsonData["per-player"];
    jsonData["players"][0]["name"] = "Alice";

    jsonData["players"][1] = jsonData["per-player"];
    jsonData["players"][1]["name"] = "Bob";
    GameState gs{&jsonData};
    //std::cout << gs.getState().dump();


    TreeNode parent(root, "root", source_code, &gs);
    identifyOperations(syntaxTree, source_code, parent, &gs);
    parent.printTree();
    // parent.execute();

    return parent;
}