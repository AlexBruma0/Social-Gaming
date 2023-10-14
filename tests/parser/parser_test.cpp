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
    std::cout << node.getType() << " Children: " << node.getNumChildren() << std::endl;
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


// executes a list of statements one at a time

void dfs(const ts::Node& node, const std::string& source_code, json& jsonData, std::vector<json>& array_lists, std::vector<int>& indexes ) {
    

    ts::Node node_to_execute = node;
    // if we see a forEach loop 
    if(node_to_execute.getType() == "rule"){
        ts::Node exeuctable_node = node_to_execute.getChild(0);
        if(exeuctable_node.getType() == "for" || exeuctable_node.getType() == "parallel_for"){
            printNode(exeuctable_node);
            const ts::Node list_node = exeuctable_node.getChild(3);
            json array_list = extractListExpression(list_node, source_code, jsonData);
            // push to array_list stack
            indexes.emplace_back(0);
            // push index = 0 to stack
            array_lists.emplace_back(array_list);
        }
    }
    // Recursively visit children nodes
    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        dfs(node.getChild(i), source_code, jsonData, array_lists, indexes);
    }
    // Revisit the node forEach loop nodes 
    if(node_to_execute.getType() == "rule"){
        ts::Node exeuctable_node = node_to_execute.getChild(0);

        if((exeuctable_node.getType() == "for" || exeuctable_node.getType() == "parallel_for") ){
           
            // check if index is greater than the size of the list
            indexes.back()++;
            std::cout<<"\nlist size: " <<array_lists.back().size()<< "\n";
            std::cout<<"\nindex: " << indexes.back()<< "\n";
            if(indexes.back() >= array_lists.back().size()){
                std::cout <<"\nfor loop over\n";
                // pop from the stack 
                array_lists.pop_back();
                indexes.pop_back();

            } else {
                
                dfs(node,source_code, jsonData, array_lists, indexes);
            }
        }
    }

}



TEST(ParserTests, RPS_TEST) {
    const int num_players = 3;
    const int num_rounds = 2;

    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);

    // Access the root node of the AST
    ts::Node root = tree.getRootNode();

    json jsonData = create_json_data(root, sourcecode);

    jsonData["configuration"]["rounds"] = num_rounds;
    for(int i = 0; i < num_players; i++){
        jsonData["players"][i] = jsonData["per-player"];
    }
    //std::cout << jsonData.dump() << "\n";
    std::cout << jsonData.dump() << "\n";
    ts::Node rules_node = root.getChildByFieldName("rules");
    ts::Node rules_node_body = rules_node.getChildByFieldName("body");
    ts::Cursor c = rules_node_body.getCursor();
    // initialize empty array_lists and indexes array
    std::vector<json> array_lists{};
    std::vector<int> indexes{};
    //dfs(rules_node_body, sourcecode, jsonData, array_lists, indexes);

}
