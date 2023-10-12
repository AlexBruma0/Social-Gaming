//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODE_H
#define SOCIAL_GAMING_RULENODE_H


#include <string>
#include <vector>

// Forward declarations for classes used in RuleTrait and RuleNode
class TreeNode;
class forNode;
class discardNode;

// Define a trait for Node behavior
template <typename T>
struct RuleTrait {
    static void execute(const T& node);
    static T parse(const std::string& data);
};

template <typename T>
class RuleNode {
public:
    RuleNode(const std::string& value);

    void addChild(const TreeNode& child);
    void printTree(int depth = 0) const;
    void execute();
    static T parse(const std::string& data);

private:
    std::string type;
    std::string rule;
    std::vector<RuleNode> children;
};

// Explicit specialization for NodeTrait
template <>
struct RuleTrait<forNode> {
    static void execute(const forNode& node);
    static forNode parse(const std::string& data);
};

template <>
struct RuleTrait<discardNode> {
    static void execute(const discardNode& node);
    static discardNode parse(const std::string& data);
};

#endif //SOCIAL_GAMING_RULENODE_H
