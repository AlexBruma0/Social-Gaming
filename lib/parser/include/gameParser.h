//
// Created by kevin on 9/30/2023.
//

#ifndef SOCIAL_GAMING_GAMEPARSER_H
#define SOCIAL_GAMING_GAMEPARSER_H

#include <string>
#include "parser.h" // Include the necessary tree-sitter header here

// read a file into a string
std::string file_to_string(const std::string path);

// parse a tree string into a syntax tree
ts::Tree string_to_tree(const std::string tree_string);

// depth first search for a tree
void dfs(const ts::Node& node, const std::string& source_code);

#endif //SOCIAL_GAMING_GAMEPARSER_H
