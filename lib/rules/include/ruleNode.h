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
#include "cpp-tree-sitter.h"

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
        TreeNode(const ts::Node& tsNode, std::string type, const std::string& sourceCode , GameState* gameState);

        TreeNode(TreeNode&& other) noexcept;

        virtual ~TreeNode(){};

        void printTree(int depth = 0) const;

        void addChild(std::unique_ptr<TreeNode> child) const; 

        void update();

        virtual void execute() const;

        std::unique_ptr<TreeNodeImpl> parseNode(const ts::Node tsNode, GameState *gameState, const std::string &source_code);

    protected:
        std::unique_ptr<TreeNodeImpl> impl;
        std::string nodeType;
};

class TreeNodeImpl { 

    public:
        // Will change to string view when refactors are made
        inline const static std::string COLLECTION_ID = "collection";
        inline const static std::string VARIABLE_ID = "var";
        inline const static std::string OPERAND_ID = "operand";
        inline const static std::string TARGET_ID = "target";
        inline const static std::string VALUE_ID = "value";

        inline const static std::string NULL_STRING = "";

        TreeNodeImpl(std::string id, GameState* gameState);
        TreeNodeImpl();
        virtual~TreeNodeImpl();

        void printTree(int depth = 0) const;

        void addChild(std::unique_ptr<TreeNode> child);

        // old data storage
        void setIdentifierData(const json& data);
        json getIdentifierData() const;

        json* getGameStateData();

        // new data storage
        void setNodeVariables(const GameVariables& data);
        GameVariables getNodeVariables() const;

        std::string getMessage();
        void eraseMessage();

        virtual void update();

        virtual void execute();


    protected:
        // Node list of children 
        std::vector<std::unique_ptr<TreeNode>> children;
        // json object to store the necessary data for each node
        json identifiers;

        GameVariables nodeVariables;

        // Contains the indexes for how to access the json objects
        // Ex json["players"][0]
        // players is stored in identifiers and 0 is stored here 
        json idIndexes;

        std::string content;
        // common to all nodes
        GameState* gameState;
};

class ForNodeImpl: public TreeNodeImpl{
public:
    ForNodeImpl(std::string id, GameState* gameState);
    ~ForNodeImpl(){}
    void execute();
    void update();


};

class DiscardNodeImpl: public TreeNodeImpl{
public:
    DiscardNodeImpl(std::string id, GameState* gameState);
    ~DiscardNodeImpl(){}
    void execute();

private:
    bool extractSize(const std::string_view operand);
};

class MessageNodeImpl: public TreeNodeImpl{
public:
    MessageNodeImpl(std::string id, GameState* gameState);
    ~MessageNodeImpl(){}
    void execute();
};

class ParallelForNodeImpl: public TreeNodeImpl {
public:
    ParallelForNodeImpl(std::string id, GameState* gameState);
    ~ParallelForNodeImpl() {}
    void execute();
};

class InputChoiceNodeImpl: public TreeNodeImpl{
public:
    InputChoiceNodeImpl(std::string id, GameState* gameState);
    ~InputChoiceNodeImpl(){}
    void execute();
};

class ScoresNodeImpl: public TreeNodeImpl{
public:
    ScoresNodeImpl(std::string id, GameState* gameState);
    ~ScoresNodeImpl(){}
    void execute();
};

class AssignmentNodeImpl: public TreeNodeImpl{
public:
    AssignmentNodeImpl(std::string id, GameState* gameState);
    ~AssignmentNodeImpl(){}
    void execute();
};

class MatchNodeImpl: public TreeNodeImpl{
public:
    MatchNodeImpl(std::string id, GameState* gameState);
    ~MatchNodeImpl(){}
    void execute();
};

class MatchEntryNodeImpl: public TreeNodeImpl{
public:
    MatchEntryNodeImpl(std::string id, GameState* gameState);
    ~MatchEntryNodeImpl(){}
    void execute();
};

class ExtendNodeImpl: public TreeNodeImpl{
public:
    ExtendNodeImpl(std::string id, GameState* gameState);
    ~ExtendNodeImpl(){}
    void execute();
};


#endif //SOCIAL_GAMING_RULENODE_H
