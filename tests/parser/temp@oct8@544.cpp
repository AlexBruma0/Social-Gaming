#include <gtest/gtest.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>

// Wrapper API
#include <cpp-tree-sitter.h>

// Custom Library in lib
#include "parser.h"

// Game Parser functionality
#include "gameParser.h"

// helper methods
#include "utils.h"

// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#define EMPTY_GAME_LOCATION "resources/games/empty.game"

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}
void printCursor(const ts::Cursor& c) {
    ts::Node node = c.getCurrentNode();
    std::cout << "Node Type: " << node.getType() << " Children: " << node.getNumChildren() << std::endl;
}
json generateNumbersList(int start, int end) {
    json numbers;
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}

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
        strings.emplace_back(get_node_value(listExpressionFirstNode, source_code));
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
        json json_return = findObjectWithStringArray(strings, jsonObj);
        output = generateNumbersList(1, json_return);

    } else{
        json json_return = findObjectWithStringArray(strings, jsonObj);
        output = json_return;
    }

    return output;
}

void executeListOfStatements(ts::Cursor& c, const std::string& source_code,const json jsonObj);

void executeForEachLoop(const std::string &variable, json list,ts::Cursor& c, const std::string& source_code,const json jsonObj ) {
    // Traverse the list of elements and execute the statement list for each element
    std::cout << "\n";
    for (const auto &element : list) {
        std::cout << "Executing for " << variable << " = " << element << ":\n";
        c.gotoFirstChild();
        c.gotoFirstChild();
        c.gotoNextSibling();
        c.gotoNextSibling();
        c.gotoNextSibling();
        c.gotoNextSibling();
        c.gotoNextSibling();
        c.gotoFirstChild();
        executeListOfStatements(c,source_code, jsonObj);
        std::cout << "Statement executed for " << variable << " = " << element << "\n";
    }
}

// executes a list of statements one at a time
void executeListOfStatements(ts::Cursor& c, const std::string& source_code,const json jsonObj) {
    
    ts::Node node_to_execute = c.getCurrentNode();
    if(node_to_execute.getType() == "rule"){

        ts::Node exeuctable_node = node_to_execute.getChild(0);
        if(exeuctable_node.getType() == "for" || exeuctable_node.getType() == "parallel_for"){
            std::cout << "\n executing for each loop...\n";
            ts::Node variable_node = exeuctable_node.getChild(1);
            std::string variable = get_node_value(variable_node, source_code);
            const ts::Node list_node = exeuctable_node.getChild(3);

            json list_array = extractListExpression(list_node, source_code, jsonObj);
            executeForEachLoop(variable, list_array, c,source_code, jsonObj);
        }
    }

    while(c.gotoNextSibling()){
        std::cout << "\n going to next node \n";
        executeListOfStatements(c,source_code, jsonObj);
    }

}



TEST(ParserTests, RPS_TEST) {
    const int num_players = 3;
    const int num_rounds = 2;

    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();
    //ts::Cursor c = root.getCursor();
    // printCursor(c);
    // std::cout << c.gotoFirstChild();
    // printCursor(c);
    // std::cout << c.gotoNextSibling();
    // printCursor(c);
    // std::cout << c.gotoFirstChild();
    // printCursor(c);
    // std::cout << c.gotoNextSibling();
    // printCursor(c);
    // std::cout << c.gotoFirstChild();
    // std::cout << c.gotoNextSibling();

    json jsonData = create_json_data(root, sourcecode);
    //std::cout << jsonData.dump() << "\n";
    // Initialize the data for each player
    //std::cout << jsonData["constants"]["weapons"][0]["name"]; //"rock"

    jsonData["configuration"]["rounds"] = num_rounds;
    for(int i = 0; i < num_players; i++){
        jsonData["players"][i] = jsonData["per-player"];
    }
    //std::cout << jsonData.dump() << "\n";

    ts::Node rules_node = root.getChildByFieldName("rules");
    ts::Node rules_node_body = rules_node.getChildByFieldName("body");
    ts::Cursor c = rules_node_body.getCursor();
    c.gotoFirstChild();
    executeListOfStatements(c, sourcecode, jsonData);
    // // Access and print the JSON data
    // for (const auto& weapon : jsonData["weapons"]) {
    //     std::cout << "Weapon: " << weapon["name"] << ", Beats: " << weapon["beats"] << "\n";
    // }


}
