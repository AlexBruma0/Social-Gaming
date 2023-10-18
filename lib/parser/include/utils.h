#ifndef UTILS_H
#define UTILS_H
#include <cpp-tree-sitter.h>
#include "parser.h" // Include the necessary tree-sitter header here
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void printNode(const ts::Node& node);
void print_bfs(const ts::Node& node);
void print_node_value(const ts::Node& node, const std::string& source_code);
std::string get_node_value(const ts::Node& node, const std::string& source_code);
bool isPunctuation(char c);
std::string removeCharacterAtIndex(const std::string& str, size_t k);
std::string RemoveLastNonSpaceBeforeClosingBracket(const std::string& str);
void replaceSubstring(std::string& str, const std::string& substr, const std::string& replacement);
void deleteCommaInRegularExpression(std::string& str);
std::string formatString(std::string& input);
json create_json_data(ts::Node root, const std::string& sourcecode);
// gets the value for the substring given by the byte range
json generateNumbersList(int start, int end);
std::string getSubstringByByteRange(const std::string &input, size_t startByte, size_t endByte);
void printCursor(const ts::Cursor& c);
json findObjectWithStringArray(std::vector<std::string> strings, json jsonObj);
json extractListExpression(const ts::Node &listExpressionNode, const std::string& source_code, json jsonObj);

#endif // UTILS_H
