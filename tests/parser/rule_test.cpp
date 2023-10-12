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
    TreeNode t("test");
    ASSERT_EQ(t.value, "test");
    ASSERT_EQ(t.children.size(), 0);
}

TEST (RuleTests, RULE_NODE_INSTANTIATE) {
    RuleNode r("test");
    TreeNodeTraverser t(r);
    t.execute();
}
