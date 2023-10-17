//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H


#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <gtest/gtest_prod.h>

// Forward declarations for classes used in RuleTrait and RuleNode
class ForNode;
class discardNode;
class TreeNode;
template <typename T> class TreeNodeImpl;

// Define a trait for Node behavior
template <typename T>
struct NodeTrait {
    static void execute(const T& node);
    static T parse(const std::string& data);
};

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
// Changed to no template so that the children nodes can be put in one list
// Coulnd't iterate over a Treenode<T> vector because the t could be different types
class TreeNode {
    public:
        TreeNode(){}

        // Destructor needed if we cast a derrived class into a treenode pointer then destruct it
        // Might not need but I'll leave just in case
        virtual ~TreeNode(){};
        void printTree(int depth = 0) const;

        // Unique pointers now
        // Should help to not have to keep track of memory
        void addChild(TreeNode* child);

        // Function to update the identifier value if something changes
        // Might be needed for for loop nodes 
        void updateIdentifier(TreeNode* child);
    private:

        // Underlying Implmentation
        // Not sure if we should have this format
        // Might be better to have just the treenode, but this will add flexibility 
        std::unique_ptr<TreeNodeImpl> impl;
        
        // Gtest to test private fields
        FRIEND_TEST(RuleTests, BASE_CLASS_INSTANTIATE);
        FRIEND_TEST(RuleTests, TREE_NODE_CHILDREN);
};

class TreeNodeImpl: public TreeNode { 
    public:
        void printTree(int depth = 0) const;

        // Unique pointers now
        // Should help to not have to keep track of memory
        void addChild(TreeNode* child);

        void updateIdentifier(TreeNode* child);

    private:

        // Node list of children 
        std::vector<TreeNode*> children;

        // Identifier to for the json object
        std::string identifier;

        // Underlying Implmentation
        // Not sure if we should have this format
        // Might be better to have just the treenode, but this will add flexibility 
        std::unique_ptr<TreeNodeImpl> impl;
}


/*

TEMPORARILY COMMENTED OUT

*/


/*
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
