
#include "parser.h"
#include "utils.h"
#include <iostream>
#include <cstdio>
#include <cpp-tree-sitter.h>
#include <exception>
#include <fstream>


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

void dfs(const ts::Node& node, const std::string& source_code) {
    // Skip nodes with type "comment"
    if (node.getType() == "comment") {
        return;
    }

    // Print out all information about a node
    std::cout << "Node Type: " << node.getType() << "               " << node.getNumChildren() << std::endl;
    //std::cout << "byte range: " << node.getByteRange().start << ", " << node.getByteRange().end << std::endl;
    // Pring substring if type is quoted_string or number

    //if(node.getType() == "quoted_string" || node.getType() == "number"){
        std::cout << "substring: " << getSubstringByByteRange(source_code, node.getByteRange().start, node.getByteRange().end ) << std::endl;
    //}
    std::cout << "num of children: " << node.getNumChildren() << std::endl << std::endl;

    // Recursively visit children nodes
    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        dfs(node.getChild(i), source_code);
    }
}