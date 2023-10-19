#include "utils.h"
#include "parser.h"
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <regex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <cctype>
#include <regex>
#include <cpp-tree-sitter.h>
#include "gameParser.h"

using json = nlohmann::json;

extern "C" {
    TSLanguage *treeSitterSocialGaming();
}

std::string getSubstringByByteRange(const std::string& input, size_t startByte, size_t endByte) {
    if (startByte >= input.size())
        throw std::runtime_error("Start byte exceeds source code size, invalid range");

    endByte = std::min(endByte, input.size() - 1);
    size_t startIndex = startByte;
    size_t length = endByte - startByte;

    return input.substr(startIndex, length);
}
// prints the type and number of children for a given node
void printNodeType(const ts::Node& node) {
    std::cout << "" << node.getType() << " Children: " << node.getNumChildren() << std::endl;
}
// prints the type and number of children for each child of a given node
void printBfs(const ts::Node& node) {
    for (int i = 0; i < node.getNumChildren(); i++) {
        printNodeType(node.getChild(i));
    }
}
// prints the string representation of a node
void printNodeStringValue(const ts::Node& node, const std::string& sourceCode) {
    if(sourceCode.empty()){
        std::cerr << "Source code is empty\n";
        return;
    }
    std::cout << getNodeStringValue(node, sourceCode) << "\n";
}
// returns a string representation of a node
std::string getNodeStringValue(const ts::Node& node, const std::string& sourceCode) {
    if (sourceCode.empty()){
        std::cerr << "Source code is empty\n";
    }
    return getSubstringByByteRange(sourceCode, node.getByteRange().start, node.getByteRange().end);
}

bool isPunctuation(char c) {
    return ispunct(c) && c != '"';
}
// removes character at index k in in a string
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
// used to format JSON string, removes any unneccesary character before closing bracket. 
std::string removeLastNonSpaceBeforeClosingBracket(const std::string& str) {
    char lastNonSpace = '\0';  // Initialize to null character

    for (int i = str.length() - 1; i >= 0; --i) {
        if (str[i] == ']') {
            for (int k = i - 1; k >= 0; --k) {
                if (str[k] != ' ' && str[k] != '\n' && str[k] == ',') {
                    std::string result = removeCharacterAtIndex(str, k);
                    return result;
                }
            }
        }
    }

    return str;
}
// replaces a substring in a string with a replacement string
void replaceSubstring(std::string& str, const std::string& substr, const std::string& replacement) {
    if(str.size() == 0) return;
    std::vector<size_t> indices;
    size_t pos = str.find(substr, 0);

    while (pos != std::string::npos) {
        str.replace(pos, substr.length(), replacement);
        indices.push_back(pos);
        pos = str.find(substr, pos + replacement.length());
    }
}
// used to format a JSON string, deletes the last comma in a list so the parser wont complain
void deleteCommaInRegularExpression(std::string& str) {
    std::regex regExpr(R"(,[\s\n]*\})");

    std::smatch match;
    if (std::regex_search(str, match, regExpr)) {
        std::size_t pos = match.position();
        if (pos != std::string::npos) {
            pos = str.find(",", pos, 1);
            if (pos != std::string::npos)
                str.erase(pos, 1);
        }
    }
}
// transforms a SocialLanguge string into a valid JSON String
std::string formatString(std::string& input) {
    if(input.empty()){
        return input;
    }
    input = removeLastNonSpaceBeforeClosingBracket(input);
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
        } else {
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
    std::string substring = "\" \"";
    size_t found = output.find(substring);
    if (found != std::string::npos) {
        output.erase(found, substring.length());
        std::string toInsert = "_";
        int index = found;
        output.insert(index, toInsert);
    }
    std::string find = "\" {";
    std::string replace = "\": {";
    replaceSubstring(output, find, replace);

    find = "(";
    replace = "[";
    replaceSubstring(output, find, replace);

    find = ")";
    replace = "]";
    replaceSubstring(output, find, replace);

    find = "\n";
    replace = ",\n";
    replaceSubstring(output, find, replace);

    find = "{,";
    replace = "{";
    replaceSubstring(output, find, replace);

    find = "[,";
    replace = "[";
    replaceSubstring(output, find, replace);

    find = "},";
    replace = "}";
    replaceSubstring(output, find, replace);

    find = "\"-\"";
    replace = "-";
    replaceSubstring(output, find, replace);

    deleteCommaInRegularExpression(output);

    return output;
}

// returns a JSON object for the constants and configuration
json createJsonData(ts::Node root, const std::string& sourcecode) {
    std::string json_string{""};

    ts::Node config_node = root.getChildByFieldName("configuration");
    ts::Node constants_node = root.getChildByFieldName("constants");
    ts::Node per_player_node = root.getChildByFieldName("per_player");
    ts::Node variables_node = root.getChildByFieldName("variables");

    std::string constants_node_to_string = getNodeStringValue(constants_node, sourcecode);
    std::string config_node_to_string = getNodeStringValue(config_node, sourcecode);
    std::string per_player_node_to_string = getNodeStringValue(per_player_node, sourcecode);
    std::string variables_node_to_string = getNodeStringValue(variables_node, sourcecode);

    std::string constants_node_to_string_json = formatString(constants_node_to_string);
    std::string config_node_to_string_json = formatString(config_node_to_string);
    std::string per_player_node_to_string_json = formatString(per_player_node_to_string);
    std::string variables_node_to_string_json = formatString(variables_node_to_string);

    json_string.append(config_node_to_string_json);
    json_string.append(constants_node_to_string_json);
    json_string.append(per_player_node_to_string_json);
    json_string.append(variables_node_to_string_json);

    if (json_string.at(0) != '{') {
        json_string.insert(0, "{\n");
        json_string.append("\n}");
    }
    std::string find = "}\"";
    std::string replace = "},\n\"";
    replaceSubstring(json_string, find , replace);

    json json_data = json::parse(json_string);

    return json_data;
}
// prints the node type and number of children for the current node of a cursor
void printCursor(const ts::Cursor& c) {
    ts::Node node = c.getCurrentNode();
    std::cout << node.getType() << " Children: " << node.getNumChildren() << std::endl;
}
// returns a JSON array of consecutive integers ranging from a given start int to a given end int inclusive
json generateNumbersList(int start, int end) {
    json numbers;
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}
// looks for a json variable in a json object given an array of strings
json findObjectWithStringArray(std::vector<std::string> strings, json jsonObj){

    // check if data is in constants
    if(!jsonObj["constants"][strings[0]].is_null()){
        jsonObj = jsonObj["constants"];
    }

    // check if data is in variables
    if(!jsonObj["variables"][strings[0]].is_null()){
        jsonObj = jsonObj["variables"];
    }

    json* currentObject = &jsonObj;

    for (const std::string& key : strings) {
    // Check if the key exists in the current JSON object
    auto it = currentObject->find(key);
    if (it != currentObject->end()) {
        // Key exists, update the current JSON object
        currentObject = &(*it);
    } else {
            json empty_object;
            std::cout << "Key '" << key << "' not found." << std::endl;
            return empty_object;
        }
    }
    return *currentObject;
}

// transforms expression into JSON array
json extractListExpression(const ts::Node &listExpressionNode, const std::string& source_code, json jsonObj) {

    
    bool upfromPresent = false;
    json output;

    for (size_t i = 0; i < listExpressionNode.getNumChildren(); ++i) {
        const ts::Node childNode = listExpressionNode.getChild(i);

        // Check for "upfrom"
        if (childNode.getType() == "builtin") {
            upfromPresent = true;

            // Assume that the value is a direct child of "upfrom"
            ts::Node valueNode = childNode.getChild(0);
            break;  // We found "upfrom", no need to search further
        }
    }

    const ts::Node listExpressionFirstNode = listExpressionNode.getChild(0);
    std::vector<std::string> strings{};

    if(listExpressionFirstNode.getNumChildren() == 0){
        strings.emplace_back(getNodeStringValue(listExpressionFirstNode, source_code));
    }

    for (size_t i = 0; i < listExpressionFirstNode.getNumChildren(); ++i){
        const ts::Node childNode = listExpressionFirstNode.getChild(i);
        if(childNode.getType() == "identifier"){
            std::string s = getSubstringByByteRange(source_code, childNode.getByteRange().start, childNode.getByteRange().end );
            strings.emplace_back(s);
        }else if(childNode.getType() == "expression"){
            const ts::Node childChildNode = childNode.getChild(0);
            std::string s = getSubstringByByteRange(source_code, childChildNode.getByteRange().start, childChildNode.getByteRange().end );
            strings.emplace_back(s);
        }
    }
    
    if(upfromPresent){
        output = generateNumbersList(1, findObjectWithStringArray(strings, jsonObj));

    } else{
        output = findObjectWithStringArray(strings, jsonObj);
    }

    return output;
}