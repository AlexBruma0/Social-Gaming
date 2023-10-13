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
    TreeNode t;
    ASSERT_EQ(t.value, "");
    ASSERT_EQ(t.children.size(), 0);
}

TEST (RuleTests, TREE_NODE_CHILDREN) {
    TreeNode parent;
    ASSERT_EQ(parent.children.size(), 0);
    auto child = std::make_unique<RuleNode>();
    child->value = "childString";
    parent.addChild(std::move(child));

    ASSERT_EQ(parent.children.size(), 1);
    ASSERT_EQ(parent.children[0]->value, "childString");
    if(dynamic_cast<RuleNode*>((parent.children[0].get()))){
        auto ptr = dynamic_cast<RuleNode*>((parent.children[0].get()));
        TreeNodeTraverser t(*ptr);
        t.execute();
    }
    
}

