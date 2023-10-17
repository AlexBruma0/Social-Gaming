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

// Rule Library
#include "ruleNode.h"

// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#define EMPTY_GAME_LOCATION "resources/games/empty.game"

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

TEST (RuleTests, BASE_CLASS_INSTANTIATE) {
    std::string nodeTest = "test";
    TreeNode t (nodeTest);
    ASSERT_EQ(t.impl->identifier, nodeTest);
    t.execute();
}

TEST (RuleTests, TREE_NODE_CHILDREN) {
    TreeNode parent("parent");
    ASSERT_EQ(parent.children.size(), 0);
    std::shared_ptr<RuleNode> child = std::make_shared<RuleNode>();
    child->value = "childString";
    parent.addChild(child);

    std::shared_ptr<ForNode> child2 = std::make_shared<ForNode>();
    child2->value = "childString2";
    parent.addChild(child2);

    ASSERT_EQ(parent.children.size(), 2);
    ASSERT_EQ(parent.children[0]->value, "childString");
    ASSERT_EQ(parent.children[1]->value, "childString2");

    // Dynamic testing
    // Will probably create a convert function
    // dynamic casting does not appear to work with other types of nodes yet.
    std::shared_ptr<RuleNode> ruleNodePtr = std::dynamic_pointer_cast<RuleNode>(parent.children[0]);

    if (ruleNodePtr) {
        TreeNodeTraverser<RuleNode> t(ruleNodePtr);
        t.execute();
    }


}

