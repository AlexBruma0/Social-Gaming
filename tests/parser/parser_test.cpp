#include <gtest/gtest.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <any>
#include <regex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <cctype>
#include <regex>
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

using json = nlohmann::json;
extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

std::string jsonString = R"(
{
    "weapons": [
        { "name": "Rock", "beats": "Scissors" },
        { "name": "Paper", "beats": "Rock" },
        { "name": "Scissors", "beats": "Paper" }
    ]
})";
void printNode (const ts::Node& node){
std::cout << "Node Type: " << node.getType() << "              Children: " << node.getNumChildren() << std::endl;
}

void print_bfs(const ts::Node& node){
    for(int i = 0; i < node.getNumChildren(); i++){
        printNode(node.getChild(i));
    }
}
void print_node_value(const ts::Node& node, const std::string& source_code){
    std::cout << getSubstringByByteRange(source_code, node.getByteRange().start,node.getByteRange().end ) << "\n";
}
std::string get_node_value(const ts::Node& node, const std::string& source_code){
    return getSubstringByByteRange(source_code, node.getByteRange().start,node.getByteRange().end );
}
bool isPunctuation(char c) {
    return ispunct(c) && c != '"';
}
std::string removeCharacterAtIndex(const std::string& str, size_t k) {
    if (k >= str.length()) {
        std::cerr << "Index out of range\n";
        return str;
    }

    std::string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (i != k) {
            result += str[i];
        }
    }

    return result;
}
std::string  RemoveLastNonSpaceBeforeClosingBracket(const std::string& str) {
    char lastNonSpace = '\0';  // Initialize to null character

    for (int i = str.length() - 1; i >= 0; --i) {
        if (str[i] == ']'){
            for(int k = i - 1; k >= 0; --k){
                if (str[k] != ' ' && str[k] != '\n' && str[k] == ',') {
                    std::string result = removeCharacterAtIndex(str,k);
                    return result;
                }
            }

        }



    }

    return str;
}

void replaceSubstring(std::string& str, const std::string& substr, const std::string& replacement) {
    std::vector<size_t> indices;
    size_t pos = str.find(substr, 0);

    while (pos != std::string::npos) {
        str.replace(pos, substr.length(), replacement);
        indices.push_back(pos);
        pos = str.find(substr, pos + replacement.length());
    }

}
void deleteCommaInRegularExpression(std::string& str) {

    // Modify the regular expression pattern
    std::regex regExpr(R"(,[\s\n]*\})");

    std::smatch match;
    if (std::regex_search(str, match, regExpr)) {
        // Replace the first occurrence of "," in the match
        std::size_t pos = match.position();
        if (pos != std::string::npos) {
            pos = str.find(",", pos, 1);  // Find the first ","
            if (pos != std::string::npos)
                str.erase(pos, 1);  // Erase the ","
        }
    }

}

std::string formatString(std::string& input) {
    input = RemoveLastNonSpaceBeforeClosingBracket(input);
    std::istringstream iss(input);
    std::ostringstream oss;
    
    bool inQuotes = false;

    char c;
    while (iss.get(c)) {
        if (isspace(c) && !inQuotes) {
            oss.put(c);
        } else if (isPunctuation(c) || isdigit(c)) {
            oss.put(c);
        } else if (c == '"') {
            oss.put(c);
            inQuotes = !inQuotes;
        }
         else {
            if (inQuotes) {
                oss.put(c);
            } else {
                oss.put('"');
                oss.put(c);
                while (iss.get(c) && !isspace(c) && !isPunctuation(c)) {
                    oss.put(c);
                }
                oss.put('"');
                iss.unget();
            }
        }
    }
    std::string output = oss.str();
    std::string substring= "\" \"";
    size_t found = output.find(substring);
    if (found != std::string::npos){
        output.erase(found, substring.length());
        std::string to_insert = "_";
        int index = found; 
        output.insert(index, to_insert);
    }
    std::string find = "\" {";
    std::string replace = "\": {";
    replaceSubstring(output, find , replace);

    find = "(";
    replace = "[";
    replaceSubstring(output, find , replace);

    find = ")";
    replace = "]";
    replaceSubstring(output, find , replace);

    find = "\n";
    replace = ",\n";
    replaceSubstring(output, find , replace);

    find = "{,";
    replace = "{";
    replaceSubstring(output, find , replace);

    find = "[,";
    replace = "[";
    replaceSubstring(output, find , replace);

    find = "},";
    replace = "}";
    replaceSubstring(output, find , replace);



    if(output.at(0) != '{'){
        output.insert(0,"{");
        output.append("}");
    }

    deleteCommaInRegularExpression(output);
    
    return output;
}

TEST(ParserTests, RPS_TEST) {
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();
    ts::Node config_node = root.getChildByFieldName("configuration");
    ts::Node constants_node = root.getChildByFieldName("constants");

    std::string constants_node_to_string = get_node_value(constants_node.getChild(1), sourcecode);
    std::string config_node_to_string = get_node_value(config_node, sourcecode);
    std::string formattedString = formatString(constants_node_to_string);
    std::cout << formattedString << "\n";
    json jsonData = json::parse(formattedString);
    // std::cout << jsonData["weapons"][0]["name"]; //"rock"
    // // Access and print the JSON data
    // for (const auto& weapon : jsonData["weapons"]) {
    //     std::cout << "Weapon: " << weapon["name"] << ", Beats: " << weapon["beats"] << "\n";
    // }


}
