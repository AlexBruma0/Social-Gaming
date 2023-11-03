#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H

#pragma once
#include "parser.h"
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <variant>
#include <gtest/gtest_prod.h>
#include <nlohmann/json.hpp>
#include "../../gameState/include/GameState.h"


// Forward declarations for classes used in RuleTrait and RuleNode

class TreeNode;
class TreeNodeImpl;

/*
    Overall Format
    Treenodes will be the node class
        Needed to be a general class so that the children vector can have a poly type
    Each tree node will have a children list pointing to other nodes
        Needs to be pointers because they could be a different type
        Can possibly change to a has a realtion
            Was thinking about using an enum field in the tree node class instead of poly
            Kept it as a type because I'm not sure if you need it as a type
    When traversing first wrap the treenode with the traverser so its TreeNodeTraverser<NodeType>
    The traverser has a node field that will have the actual node data
        Execute and parse will now manipulate that node field
*/

using jsonReturnFormat = std::variant<std::vector<std::string>, std::vector<size_t>>;

// Base class to implement all other node types
// Abstraction so that the parser doesn't have to deal with the underlying node stuff
// Based on Professor Sumner's client design in the networking class
class TreeNode {
    public:
        TreeNode(std::string node, std::string type, GameState& gameState);

        TreeNode(TreeNode&& other) noexcept;

        virtual ~TreeNode(){};

        void printTree(int depth = 0) const;


        void addChild(std::unique_ptr<TreeNode> child) const; 

        // Update the indexes
        // Needs to be here because the TreeNodeImpls access treeNodes
        void update();

        // Function to update the identifier value if something changes

        virtual void execute() const;

        std::unique_ptr<TreeNodeImpl> parseNode(const std::string& node, GameState& gameState);

    protected:
        std::unique_ptr<TreeNodeImpl> impl;
        std::string nodeType;
        
};

class TreeNodeImpl { 

    public:
        TreeNodeImpl(std::string id, GameState& gameState);
        TreeNodeImpl();
        virtual~TreeNodeImpl();

        void printTree(int depth = 0) const;

        void addChild(std::unique_ptr<TreeNode> child);

        void setIdentifierData(const json& data);

        json getIdentifierData() const;

        virtual void update();

        virtual void execute();


    protected:
        // Node list of children 
        std::vector<std::unique_ptr<TreeNode>> children;
        // json object to store the necessary data for each node
        json identifiers;

        // Contains the indexes for how to access the json objects
        // Ex json["players"][0]
        // players is stored in identifiers and 0 is stored here 
        json idIndexes;

        std::string content;
        // common to all nodes
        GameState gameState;
};

class ForNodeImpl: public TreeNodeImpl{
public:
    ForNodeImpl(std::string id, GameState& gameState);
    ~ForNodeImpl(){}
    void execute();
    void update();


};

class DiscardNodeImpl: public TreeNodeImpl{
public:
    DiscardNodeImpl(std::string id, GameState& gameState);
    ~DiscardNodeImpl(){}
    void execute();
};

class MessageNodeImpl: public TreeNodeImpl{
public:
    MessageNodeImpl(std::string id, GameState& gameState);
    ~MessageNodeImpl(){}
    void execute();
};

class ParallelForNodeImpl: public TreeNodeImpl {
public:
    ParallelForNodeImpl(std::string id, GameState &gameState);
    ~ParallelForNodeImpl() {}
    void execute();
};

class InputChoiceNodeImpl: public TreeNodeImpl{
public:
    InputChoiceNodeImpl(std::string id, GameState& gameState);
    ~InputChoiceNodeImpl(){}
    void execute();
};

#endif //SOCIAL_GAMING_RULENODE_H
