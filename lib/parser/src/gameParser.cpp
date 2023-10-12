
#include "parser.h"
#include <iostream>
#include <cstdio>
#include <cpp-tree-sitter.h>
#include <vector>
#include <exception>
#include <algorithm>

extern "C" {
    //TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

std::string file_to_string(const std::string path) {
    FILE* f = fopen(path.c_str(), "r");
    if(!f){
        printf("Could not find file\n");
        return std::string();
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);

    char game_string[size];
    rewind(f);
    fread(game_string, sizeof(char), size, f);

    fclose(f);
    return game_string;
}

ts::Tree string_to_tree(const std::string tree_string) {
    // Create a language and parser
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};

    // Parse the provided serialized string into a syntax tree and return
    return parser.parseString(tree_string);
}

std::string getSubstringByByteRange(const std::string& input, size_t startByte, size_t endByte) {
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
    return getSubstringByByteRange(source_code, node.getByteRange().start, node.getByteRange().end);
}

void dfs(const ts::Node& node, const std::string& source_code) {
    // valid rule types
    std::vector<std::string_view> allowedTypes = {
            "for", "loop", "parallel_for", "in_parallel", "match", "extend", "reverse", "shuffle",
            "sort", "deal", "discard", "assignment", "timer", "input_choice", "input_text", "input_vote",
            "input_range", "message", "scores"
    };

    // Skip nodes with type "comment"
    if (node.getType() == "comment") {
        return;
    }
    auto it = std::find(allowedTypes.begin(), allowedTypes.end(), node.getType());
    if (it != allowedTypes.end()) {
        std::cout << "NODE FOUND of type " << node.getType() << std::endl;
        std::cout << getSubstringForNode(node, source_code) << std::endl;
    }

    // Recursively visit children nodes
    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        dfs(node.getChild(i), source_code);
    }
}