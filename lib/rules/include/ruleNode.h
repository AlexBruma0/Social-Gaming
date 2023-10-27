//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H

#include "parser.h"
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <gtest/gtest_prod.h>
#include <nlohmann/json.hpp>
#include "../../gameState/include/GameState.h" 


// Forward declarations for classes used in RuleTrait and RuleNode

class TreeNode;
class TreeNodeImpl;
class ForNode;
class RuleNode;

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

// Base class to implement all other node types
// Abstraction so that the parser doesn't have to deal with the underlying node stuff
// Based on Professor Sumner's client design in the networking class
class TreeNode {
    public:
        TreeNode(std::string node, std::string type, GameState& gameState);

        TreeNode(TreeNode&& other) noexcept;

        ~TreeNode(){};

        void printTree(int depth = 0) const;


        void addChild(std::unique_ptr<TreeNode> child) const; 

        // Function to update the identifier value if something changes

        void execute() const;

        std::unique_ptr<TreeNodeImpl> parseNode(const std::string& node, GameState& gameState);

    private:
        std::unique_ptr<TreeNodeImpl> impl;
        std::string nodeType;
        
        // Gtest to test private fields
        FRIEND_TEST(RuleTests, BASE_CLASS_INSTANTIATE);
        FRIEND_TEST(RuleTests, TREE_NODE_CHILDREN);
};

class TreeNodeImpl { 

    public:
        TreeNodeImpl(std::string id, GameState& gameState);
        TreeNodeImpl();
        ~TreeNodeImpl();

        void printTree(int depth = 0) const;

        void addChild(std::unique_ptr<TreeNode> child);

        void setIdentifierData(const json& data);

        json getIdentifierData() const;

        virtual void execute();


    protected:
        // Node list of children 
        std::vector<std::unique_ptr<TreeNode>> children;

        // json object to store the necessary data for each node
        json identifiers;

        std::string content;

        // common to all nodes
        GameState gameState;

        // Gtest to test private fields
        FRIEND_TEST(RuleTests, BASE_CLASS_INSTANTIATE);
        FRIEND_TEST(RuleTests, TREE_NODE_CHILDREN);
};

class ForNodeImpl: public TreeNodeImpl{
public:
    ForNodeImpl(std::string id, GameState& gameState);
    ~ForNodeImpl(){}
    void execute();

private:
    std::vector<std::unique_ptr<TreeNode>> children;
    json identifiers;
    std::string content;
    GameState gameState;
};

class DiscardNodeImpl: public TreeNodeImpl{
public:
    DiscardNodeImpl(std::string id, GameState& gameState);
    ~DiscardNodeImpl(){}
    void execute();

private:
    std::vector<std::unique_ptr<TreeNode>> children;
    json identifiers;
    std::string content;
    GameState gameState;
};

class InputChoiceNodeImpl: public TreeNodeImpl{
public:
    InputChoiceNodeImpl(std::string id, GameState& gameState);
    ~InputChoiceNodeImpl(){}
    void execute();

private:
    std::vector<std::unique_ptr<TreeNode>> children;
    json identifiers;
    std::string content;
    GameState gameState;
};

#endif //SOCIAL_GAMING_RULENODE_H
