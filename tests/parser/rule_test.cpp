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

#define DUMMY_ID "DUMMY"

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

class dummyNode: public TreeNodeImpl{
    public:
        dummyNode(GameState& gameState, json& indexes): TreeNodeImpl("dummy", gameState){
            idIndexes = indexes;
        }
        void execute(){
            testCounter += idIndexes[DUMMY_ID].get<size_t>();
        }

        size_t getCounter(){
            return testCounter;
        }

        int getIndex(){
            return idIndexes[DUMMY_ID].get<int>();
        }

    private:
        size_t testCounter = 0;
};

class childNode: public TreeNode{
    public:
        childNode(std::string node, std::string type, GameState& gameState, std::unique_ptr<dummyNode> d): TreeNode(node, type, gameState){
            impl = std::move(d);
        }

        dummyNode* getImpl(){
            return dynamic_cast<dummyNode*>( impl.get());
        }
        
        MOCK_METHOD(void, execute, (), ());
};
class ForNodeMock :public ForNodeImpl{
    public:
        ForNodeMock(GameState& gameState,std::unique_ptr<childNode> t): ForNodeImpl("id", gameState){
       

            addChild(std::move(t));
        }

        childNode* getC(){
            return dynamic_cast<childNode*>( ForNodeImpl::children[0].get());
        }

        MOCK_METHOD(jsonReturnFormat, getJSON, (std::string id), ());
        MOCK_METHOD(void, execute, (), ());

};



TEST (RuleTests, forNodeExecute){
    int vecSize = 6;
    auto vecData = std::vector<int>(vecSize);
    std::iota(vecData.begin(), vecData.end(),0);
    auto sum = std::accumulate(vecData.begin(), vecData.end(), 0);

    json j; 
    j[DUMMY_ID] = vecData;

    json indexes;
    indexes[DUMMY_ID] =0;
    
    GameState gs{j};
    auto dummy = std::make_unique<dummyNode> (gs, indexes);
    auto child = std::make_unique<childNode>("bad", "bad",gs, std::move(dummy));

    ForNodeMock fNode(gs, std::move(child));
    fNode.setIdentifierData(j);
    
    EXPECT_CALL(fNode, execute()).WillOnce([&fNode]{
        return fNode.ForNodeImpl::execute();
    });
    
    fNode.execute();

    auto counter = fNode.getC()->getImpl()->getCounter();
    auto index = fNode.getC()->getImpl()->getIndex();
    ASSERT_EQ(counter, sum);
    ASSERT_EQ(index, (vecSize-1));

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

