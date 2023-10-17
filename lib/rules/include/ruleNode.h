//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H


#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <gtest/gtest_prod.h>

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

// Base class to implement all other node types
// Abstraction so that the parser doesn't have to deal with the underlying node stuff
// Based on Professor Sumner's client design in the networking class
class TreeNode {
    public:
        TreeNode(std::string node);

        ~TreeNode(){};

        void printTree(int depth = 0) const;


        void addChild(const TreeNode* child) const;

        // Function to update the identifier value if something changes

        // Might be needed for for loop nodes 
        void updateIdentifier(const std::string& identifier);

        void execute() const;

        std::unique_ptr<TreeNodeImpl> parseNode(const std::string& node);
    private:
        std::unique_ptr<TreeNodeImpl> impl;
        
        // Gtest to test private fields
        FRIEND_TEST(RuleTests, BASE_CLASS_INSTANTIATE);
        FRIEND_TEST(RuleTests, TREE_NODE_CHILDREN);
};

class TreeNodeImpl { 

    public:
        TreeNodeImpl(std::string id);
        ~TreeNodeImpl();

        void printTree(int depth = 0) const;

        void addChild(const TreeNode* child);

        void updateIdentifier(const std::string& identifier);

        void execute();

    private:

        // Node list of children 
        std::vector<const TreeNode*> children;

        // Identifier to for the json object
        std::string identifier;

        // Gtest to test private fields
        FRIEND_TEST(RuleTests, BASE_CLASS_INSTANTIATE);
        FRIEND_TEST(RuleTests, TREE_NODE_CHILDREN);
};

class ForNode: public TreeNodeImpl{

}

/*

TEMPORARILY COMMENTED OUT




// Define a trait for Node behavior
template <typename T>
struct NodeTrait {
    static void execute(const T& node);
    static T parse(const std::string& data);
};


// Traversere class to do execute and parse on each node
// Like a wrapper class that will of form TreeNodeTraverse<RuleNode>
// Unsure if it should be a has or not
// Will experiment more
template <typename T>
class TreeNodeTraverser{
    public:
        TreeNodeTraverser(const std::shared_ptr<T>& node)
            : node(node){};

        ~TreeNodeTraverser(){};

        // Functions that every tree node should have
        // Template classes need to have their functions implemented in the .h files
        void execute() {
            //NodeTrait<T>::execute(static_cast<const T&>(node));
            NodeTrait<T>::execute(node);
        }

        // Parse data to create a node
        std::shared_ptr<T> parse(const std::string& data) {
            return NodeTrait<T>::parse(data);
        }
        private:
            T node;
};


class RuleNode: public TreeNode {
public:
    RuleNode();
    RuleNode(const std::shared_ptr<RuleNode>& sharedPtr)
            : TreeNode(), type(""), rule(""), sharedPtr_(sharedPtr) {}
    ~RuleNode();

private:
    std::string type;
    std::string rule;
    std::shared_ptr<RuleNode> sharedPtr_;
};

template <>
struct NodeTrait<RuleNode> {
    static void execute(const RuleNode& node){
        // Printing Test
        printf("I'm a ruleNode!\n");
    };
    static RuleNode parse(const std::string& data){

    };
};


class ForNode: public TreeNode {
public:
    ForNode();
    ~ForNode();

private:
    std::string type;
    std::string rule;
};

template <>
struct NodeTrait<ForNode> {
    static void execute(const ForNode& node){
        // Printing Test
        printf("I'm a ForNode!\n");
    };
    static ForNode parse(const std::string& data);
};

template <>
struct NodeTrait<discardNode> {
    static void execute(const discardNode& node);
    static discardNode parse(const std::string& data);
};
*/

#endif //SOCIAL_GAMING_RULENODE_H
