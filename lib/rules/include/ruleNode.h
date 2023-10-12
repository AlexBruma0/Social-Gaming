//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H


#include <string>
#include <vector>
#include <iostream>

// Forward declarations for classes used in RuleTrait and RuleNode
class forNode;
class discardNode;
class TreeInterface;
template <typename T> class TreeNode;

// Define a trait for Node behavior
template <typename T>
struct NodeTrait {
    static void execute(const T& node);
    static T parse(const std::string& data);
};


class TreeInterface {

};

template <typename T>
class TreeNode {
    public: 
        TreeNode(const std::string& value)
            : value(value){};

        // (cstong) Destructor needed if we cast a derrived class into a treenode pointer then destruct it
        // Might not need but I'll leave just in case
        virtual ~TreeNode(){};

        // Functions that every tree node should have
        void printTree(int depth = 0) const;
        void execute() ;
        static T parse(const std::string& data);
        void addChild(const TreeNode<T>& child);
    private: 
        std::vector<TreeNode> children;
        std::string value;

};


template <typename T>
class RuleNode: public TreeNode<T> {
public:
    RuleNode(const std::string& value);
    ~RuleNode();
    void addChild(const TreeNode<T>& child);

private:
    std::string type;
    std::string rule;
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

#endif //SOCIAL_GAMING_RULENODE_H
