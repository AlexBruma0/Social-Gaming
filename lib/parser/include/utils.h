#ifndef UTILS_H
#define UTILS_H

#include <nlohmann/json.hpp>
#include <cpp-tree-sitter.h>
#include "parser.h"
#include <string>
#include "GameState.h"


using json = nlohmann::json;

void printNodeType(const ts::Node& node);

void printBfs(const ts::Node& node);

void printNodeStringValue(const ts::Node& node, const std::string& sourceCode);

std::string getNodeStringValue(const ts::Node& node, const std::string& sourceCode);

bool isPunctuation(char c);

std::string removeCharacterAtIndex(const std::string& str, size_t k);

std::string removeLastNonSpaceBeforeClosingBracket(const std::string& str);

void replaceSubstring(std::string& str, const std::string& substr, const std::string& replacement);

void deleteCommaInRegularExpression(std::string& str);

std::string formatString(std::string& input);

json createJsonData(ts::Node root, const std::string& sourceCode);

// TODO: make a GameVariables map out of the json that already exists
// we can probably just brute force it
GameVariables createGameVariables(json& jsonObj, GameVariables& parentGameVariables);

json generateNumbersList(int start, int end);

std::string getSubstringByByteRange(const std::string &input, size_t startByte, size_t endByte);

void printCursor(const ts::Cursor& c);

json findObjectWithStringArray(std::vector<std::string> strings, json jsonObj);

json extractListExpression(const ts::Node &listExpressionNode, const std::string& sourceCode, json jsonObj);

#endif // UTILS_H
