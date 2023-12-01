#ifndef SOCIAL_GAMING_GAMEPARSER_H
#define SOCIAL_GAMING_GAMEPARSER_H

#include <string>
#include "parser.h" // Include the necessary tree-sitter header here
#include "../../rules/include/ruleNode.h"
#include "MessageQueue.h"
#include "gameServer.h"

#pragma once

// read a file into a string
std::string file_to_string(const std::string path);

// parse a tree string into a syntax tree
ts::Tree string_to_tree(const std::string tree_string);

// gets the value for the substring given by the byte range
std::string getSubstringUsingByteRange(const std::string& input, size_t startByte, size_t endByte);

// depth first search for a tree; debugging use to see contents
void printDfs(const ts::Node& node, const std::string& source_code, int depth);

// creates a cleaner tree with nodes that store type-specific information for easier use
void identifyOperations(const ts::Node& node, const std::string& source_code, TreeNode& parentNode, GameState* gameState, SendMessageQueue* in, ReceiveMessageQueue* out);

TreeNode buildRuleTree(const ts::Node& syntaxTree, const std::string& source_code, SendMessageQueue* in, ReceiveMessageQueue* out, GameServer* server, GameState* gs);

#endif //SOCIAL_GAMING_GAMEPARSER_H
