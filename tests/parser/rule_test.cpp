#include <gtest/gtest.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <gmock/gmock.h>

// Wrapper API
#include <cpp-tree-sitter.h>

// Custom Library in lib
#include "parser.h"

// Game Parser functionality
#include "gameParser.h"

// Rule Library
#include "ruleNode.h"

#include "ruleNodeSupport.h"

// Current Directory is your build directory
#define RPS_LOCATION "resources/games/rock-paper-scissors.game"
#define EMPTY_GAME_LOCATION "resources/games/empty.game"

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

class ForNodeMock :public ForNodeImpl{
    public:
        ForNodeMock(GameState& gameState): ForNodeImpl("id", gameState){}
        MOCK_METHOD(jsonReturnFormat, getJSON, (std::string id), ());
        MOCK_METHOD(void, execute, (), ());
};

TEST (RuleTests, forNodeExecute){
    json j; 
    j["start"] = 1;
    GameState gs{j};
    ForNodeMock fNode(gs);
    std::vector<std::string> v{"1","2","3"};
    int idx = 2;
    EXPECT_CALL(fNode, execute()).WillOnce([&fNode](){
        return fNode.ForNodeImpl::execute();
    });
    fNode.execute();
}

TEST (RuleTests, BASE_CLASS_INSTANTIATE) {
    // std::string nodeTest = "test";
    // TreeNode t (nodeTest, nodeTest);
    // ASSERT_EQ(t.impl->identifier, "bad");
}
// need to rewrite tests to work with the addchild() method
TEST (RuleTests, TREE_NODE_CHILDREN) {
    // TreeNode parent("parent", "dummy");
    // ASSERT_EQ(parent.impl->children.size(), 0);

    // std::string childS = "childString";
    // TreeNode c1 ("parent2", "dummy");
    // auto child = std::make_unique<ForNodeImpl>(childS);
    // c1.impl = std::move(child);
    // parent.addChild(&c1);

    // std::string childS2 = "childString2";
    // TreeNode c2 ("parent3", "dummy");
    // auto child2 = std::make_unique<ForNodeImpl>(childS2);
    // c2.impl = std::move(child2);
    // parent.addChild(&c2);


    // ASSERT_EQ(parent.impl->children.size(), 2);

    // ASSERT_EQ(parent.impl->children[0]->impl->identifier, childS);
    // ASSERT_EQ(parent.impl->children[1]->impl->identifier, childS2);

    // // Dynamic testing
    // // Will probably create a convert function
    // // dynamic casting does not appear to work with other types of nodes yet.
    // auto ruleNodePtr = dynamic_cast<ForNodeImpl*>(parent.impl->children[0]->impl.get());
    // ASSERT_NE(ruleNodePtr, nullptr);


}

