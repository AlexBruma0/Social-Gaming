
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
        << getSubstringByByteRange(source_code, node.getByteRange().start, node.getByteRange().end)
        << std::endl;

    // Recursively visit children nodes for all nodes
    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        printDfs(node.getChild(i), source_code, depth + 1);
    }
}

// everything below here is WIP and will be moved soon! It's just here while the node structure is not yet ready
std::string getFirstLine(const std::string& input) {
    std::size_t newlinePos = input.find('\n');

    if (newlinePos != std::string::npos) {
        // found a newline, return the part before it
        return input.substr(0, newlinePos);
    } else {
        // no newline found, return the entire string
        return input;
    }
}

std::vector<std::string> splitStringBySpace(const std::string& input) {
    std::vector<std::string> tokens;
    bool insideQuotes = false;
    std::string token;

    for (char c : input) {
        if (c == '"') {
            insideQuotes = !insideQuotes;
            token += c;
        } else if (c == ' ' && !insideQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

// temporarily copied from ruleNodeSupport. I'm just doing this here, the functionality should be in there when node structure is ready.
// I'm just doing this to avoid messing with the build structure, this should be moved away asap.

// stuff that will be moved into a util file or inside the node itself (parse method). This should not stay here.
// in a real usage the strings will be decomposed into type-specific variables for consumption.
std::string processFor(const std::string op_string) {
    return (splitStringBySpace(op_string).size() > 1) ? getFirstLine(op_string) : "";
}

std::string processDiscard(const std::string op_string) {
    return (splitStringBySpace(op_string).size() == 4) ? op_string : "";
}

std::string processMessage(const std::string op_string) {
    return (splitStringBySpace(op_string).size() == 3) ? op_string : "";
}

std::string processParallelFor(const std::string op_string) {
    return (splitStringBySpace(op_string).size() > 1) ? getFirstLine(op_string) : "";
}

std::string processInputChoice(const std::string op_string) {
    return (splitStringBySpace(op_string).size() > 1) ? op_string : "";
}

std::string processMatch(const std::string op_string) {
    return (splitStringBySpace(op_string).size() > 1) ? op_string : "";
}

std::string processScores(const std::string op_string) {
    return (splitStringBySpace(op_string).size() == 2) ? op_string : "";
}

std::string processExtend(const std::string op_string) {
    return (splitStringBySpace(op_string).size() > 3) ? op_string : "";
}

// this will be used to fill in the nodes when they are ready
// when we use it for real, we will pass in a parentNode as a parameter so we know what to attach to.
void identifyOperations(const ts::Node& node, const std::string& source_code, int depth = 0, const std::string& parentNode = "root") {
    // list of stuff we need to implement, operation nodes defined by the language
//    std::vector<std::string_view> allowedTypes = {
//            "for", "loop", "parallel_for", "in_parallel", "match", "extend", "reverse", "shuffle",
//            "sort", "deal", "discard", "assignment", "timer", "input_choice", "input_text", "input_vote",
//            "input_range", "message", "scores"
//    };

    std::unordered_map<std::string, std::function<std::string(const std::string&)>> typeToFunction;
    typeToFunction["for"] = processFor;
    typeToFunction["discard"] = processDiscard;
    typeToFunction["message"] = processMessage;
    typeToFunction["parallel_for"] = processParallelFor;
    typeToFunction["input_choice"] = processInputChoice;
    typeToFunction["match"] = processMatch;
    typeToFunction["scores"] = processScores;
    typeToFunction["extend"] = processExtend;
    // add more as we continue to see more node types

    std::string nodeType = std::string(node.getType());

    // Skip nodes with type "comment"
    if (nodeType == "comment") {
        return;
    }

    if (typeToFunction.find(nodeType) != typeToFunction.end()) {
        std::string input = getSubstringForNode(node, source_code);
        std::string result = typeToFunction[nodeType](input);

        if (result == "") {
            return;
        }

        printIndents(depth);
        //std::cout << "Node of type: " << nodeType << " at depth " << depth << " with parent " << parentNode << std::endl;
        std::cout << "Node of type: " << nodeType << " at depth " << depth << " with parent type " << parentNode << std::endl;
        std::cout << result << std::endl << std::endl;

        // todo: make the node here and pass it along as a parent
    }

    // Recursively visit children nodes
    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        if (typeToFunction.find(nodeType) != typeToFunction.end()) {
            identifyOperations(node.getChild(i), source_code, depth+1, nodeType);
        } else {
            identifyOperations(node.getChild(i), source_code, depth+1, parentNode);
        }

    }
}