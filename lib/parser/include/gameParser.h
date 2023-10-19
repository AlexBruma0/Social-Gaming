//
// Created by kevin on 9/30/2023.
//

#ifndef SOCIAL_GAMING_GAMEPARSER_H
#define SOCIAL_GAMING_GAMEPARSER_H

#include <string>
#include "parser.h" // Include the necessary tree-sitter header here
#include "../../rules/include/ruleNode.h"

// read a file into a string
std::string file_to_string(const std::string path);

// parse a tree string into a syntax tree
ts::Tree string_to_tree(const std::string tree_string);

// gets the value for the substring given by the byte range
std::string getSubstringUsingByteRange(const std::string& input, size_t startByte, size_t endByte);

// depth first search for a tree; debugging use to see contents
void printDfs(const ts::Node& node, const std::string& source_code, int depth);

// creates a cleaner tree with nodes that store type-specific information for easier use
void identifyOperations(const ts::Node& node, const std::string& source_code, const TreeNode& parentNode, int depth = 0);

TreeNode buildRuleTree(const ts::Node& syntaxTree, const std::string& source_code);

#endif //SOCIAL_GAMING_GAMEPARSER_H
