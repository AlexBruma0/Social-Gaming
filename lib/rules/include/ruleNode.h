//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H


#include <string>
#include <vector>
#include <iostream>
#include <gtest/gtest_prod.h>

// Forward declarations for classes used in RuleTrait and RuleNode
class forNode;
class discardNode;
class TreeNode;
template <typename T> class TreeNodeImpl;

// Define a trait for Node behavior
template <typename T>
struct NodeTrait {
    static void execute(const T& node);
    static T parse(const std::string& data);
};

// (cstong) Base class to implement all other node types
// Changed to no template so that the children nodes can be put in one list
// Coulnd't iterate over a Treenode<T> vector because the t could be different types
class TreeNode {
    public:
        TreeNode(const std::string& value)
            : value(value){}

        // (cstong) Destructor needed if we cast a derrived class into a treenode pointer then destruct it
        // Might not need but I'll leave just in case
        virtual ~TreeNode(){};
        void printTree(int depth = 0) const;
        void addChild(const TreeNode* child);
    private:
        std::vector<const TreeNode*> children;
        std::string value;
        
        // Gtest to test private fields
        FRIEND_TEST(RuleTests, BASE_CLASS_INSTANTIATE);
};


class RuleNode: public TreeNode {
public:
    RuleNode(const std::string& value);
    ~RuleNode();

private:
    std::string type;
    std::string rule;
};



// (cstong) Traversere class to do execute and parse on each node
// Like a wrapper class that will of form TreeNodeTraverse<RuleNode> 
// Unsure if it should be a has or not 
// Will experiment more
template <typename T>
class TreeNodeTraverser{
    public: 
        TreeNodeTraverser(const T& node)
            : node(node){};

        ~TreeNodeTraverser(){};

        // Functions that every tree node should have
        // Template classes need to have their functions implemented in the .h files
        void execute() {
            printf("executing\n");
            NodeTrait<T>::execute(static_cast<const T&>(node));
        }

        // Parse data to create a node
        T parse(const std::string& data) {
            return NodeTrait<T>::parse(data);
        }
        private:
            T node;
};


// Explicit specialization for NodeTrait
template <>
struct NodeTrait<forNode> {
    static void execute(const forNode& node);
    static forNode parse(const std::string& data);
};

template <>
struct NodeTrait<discardNode> {
    static void execute(const discardNode& node);
    static discardNode parse(const std::string& data);
};

template <>
struct NodeTrait<RuleNode> {
    static void execute(const RuleNode& node){
        // Printing Test
        printf("I'm a ruleNode!\n");
    };
    static RuleNode parse(const std::string& data);
};

#endif //SOCIAL_GAMING_RULENODE_H
