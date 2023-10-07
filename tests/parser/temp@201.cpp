#include <gtest/gtest.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <any>
// #include <span>
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
struct Weapon {
    std::string name;
    std::string beats;
};

std::vector<Weapon> weapons{ {"rock", "scissors"}, {"paper","rock"}, {"scissors", "paper"} };

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}
std::unordered_map<std::string, int> configurationMap = {
    {"configuration.rounds", 4}  
};
std::unordered_map<std::string, std::vector<Weapon>> constantsMap = {
    {"weapons", weapons}  
};

std::vector<int> generateNumbersList(int start, int end) {
    std::vector<int> numbers;
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}

// Implement other parsing methods for each statement type
// Add similar methods for other statement types
void printNode (const ts::Node& node){
std::cout << "Node Type: " << node.getType() << "              Children: " << node.getNumChildren() << std::endl;
}

void print_bfs(const ts::Node& node){
    for(int i = 0; i < node.getNumChildren(); i++){
        printNode(node.getChild(i));
    }
}
std::string constructStringWithPeriods(const std::vector<std::string>& strings) {
    if (strings.empty())
        return "";

    std::string constructedString = strings[0];
    if(strings.size() == 1){
        return constructedString;
    }

    for (size_t i = 1; i < strings.size(); ++i) {
        constructedString += "." + strings[i];
    }

    return constructedString;
}
template <typename T>
T getValueFromIdentifier(const std::string& identifier) {
    auto it = configurationMap.find(identifier);
    if (it != configurationMap.end()) {
        return it->second;
    } else {
        // Handle the case where the identifier is not found
        std::cerr << "Identifier not found: " << identifier << std::endl;
        return -1;  // or some default value indicating an error
    }
}
std::vector<std::any> convertIntVectorToAnyVector(const std::vector<int>& intVector) {
    std::vector<std::any> anyVector;
    for (const auto& intValue : intVector) {
        anyVector.emplace_back(intValue);
    }
    return anyVector;
}
std::vector<std::any> convertWeaponVectorToAnyVector(const std::vector<Weapon>& weaponVector) {
    std::vector<std::any> anyVector;
    for (const auto& weapon : weaponVector) {
        std::any weaponAny = std::make_any<Weapon>(weapon);
        anyVector.emplace_back(std::move(weaponAny));
    }
    return anyVector;
}


std::vector<std::any> extractListExpression(const ts::Node &listExpressionNode, const std::string& source_code) {

    std::vector<std::any> result{};
    bool upfromPresent = false;

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
        std::string stringToLookUp = constructStringWithPeriods(strings);
        // int key = getValueFromIdentifier<int>(stringToLookUp);
        // std::vector<int> intVector = generateNumbersList(1, key);
        // result = convertIntVectorToAnyVector(intVector);
    } else{
        // std::string stringToLookUp = constructStringWithPeriods(strings);
        // auto key = getValueFromIdentifier<std::vector<Weapon>>(stringToLookUp);
        // result = convertWeaponVectorToAnyVector(key);
    }

    return result;
}

void executeStatementList(const ts::Node &sub_tree_node_to_execute, const std::string& source_code) {
    const ts::Node first_for = sub_tree_node_to_execute.getChild(1).getChild(0);
    const ts::Node list_expression = first_for.getChild(3);
    auto result = extractListExpression(list_expression, source_code);
    for (const auto& element : result) {
        try {
            if (element.type() == typeid(int))
                std::cout << "Integer: " << std::any_cast<int>(element) << std::endl;
            else if (element.type() == typeid(double))
                std::cout << "Double: " << std::any_cast<double>(element) << std::endl;
            else if (element.type() == typeid(const char*))
                std::cout << "String: " << std::any_cast<const char*>(element) << std::endl;
            else
                std::cout << "Unknown type" << std::endl;
        } catch (const std::bad_any_cast& e) {
            std::cout << "Failed to cast: " << e.what() << std::endl;
        }
    }
    

    //print_bfs(first_for.getChild(3));
    //print_bfs(first_for);
    // std::cout << config.rounds << "\n";
    // for(auto& w : weapons){
    //     std::cout << w.name << "\n";
    // }

}



TEST(ParserTests, RPS_TEST) {
    // Read file as SocialGaming code and parse into a syntax tree
    // Rules node = 17 
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();
    ts::Node rules_node = root.getChildByFieldName("rules");
    ts::Node rules_node_body = rules_node.getChildByFieldName("body");
    executeStatementList(rules_node_body, sourcecode);


}
