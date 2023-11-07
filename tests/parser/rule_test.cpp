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
#define ARRAY_ID "collection"

extern "C" {
//TSLanguage *tree_sitter_json();
    TSLanguage *tree_sitter_socialgaming();
}

// Dummy Node class
// Overrides the other classes so we can test that something happens
class dummyNode: public TreeNodeImpl{
    public:
        dummyNode(GameState* gameState, json& indexes): TreeNodeImpl("dummy", gameState){
            idIndexes = indexes;
        }
        void execute(){
            testCounter += idIndexes[ARRAY_ID].get<size_t>();
        }

        size_t getCounter(){
            return testCounter;
        }

        int getIndex(){
            return idIndexes[ARRAY_ID].get<size_t>();
        }
        void update(){
            // Temporary will be changed when parser IDs are decided
            idIndexes.front() = idIndexes.front().get<size_t>() + 1;
        }
    private:
        size_t testCounter = 0;
};

class readNode: public TreeNodeImpl{
    public:
        readNode(GameState* gameState, json& indexes, json& data): TreeNodeImpl("dummy", gameState){
            idIndexes = indexes;
            setIdentifierData(data);
        }
        void execute(){
            //std::cout<<"reading"<<std::endl;
            json* j = gameState->getState();

            testCounter += (*j)[ARRAY_ID][getIndex()].get<int>();
        }

        size_t getCounter(){
            return testCounter;
        }

        void update(){
            // Temporary will be changed when parser IDs are decided
            idIndexes.front() = idIndexes.front().get<size_t>() + 1;
        }

        int getIndex(){
            return idIndexes[ARRAY_ID].get<int>();
        }

    private:
        size_t testCounter = 0;
};


class writeNode: public TreeNodeImpl{
    public:
        writeNode(GameState* gameState, json& indexes, json& data): TreeNodeImpl("dummy", gameState){
            idIndexes = indexes;
            setIdentifierData(data);


        }
        void execute(){
            //std::cout<<"writing"<<std::endl;
            json* j = gameState->getState();
            (*j)[ARRAY_ID][getIndex()] = (*j)[ARRAY_ID][getIndex()].get<int>() +1;
        }

        void update(){
            // Temporary will be changed when parser IDs are decided
            idIndexes.front() = idIndexes.front().get<size_t>() + 1;
        }

        int getIndex(){
            return idIndexes[ARRAY_ID].get<int>();
        }
};

// ChildNode wrapper
// Need to check how many times it executes and for mocks
// Also needed to change the impl
class childNode: public TreeNode{
    public:
        childNode(const ts::Node& node, std::string type, std::string& sourceCode, GameState* gameState, std::unique_ptr<TreeNodeImpl> d):
            TreeNode(node, type, sourceCode, gameState){
            impl = std::move(d);
        }

        dummyNode* getImpl(){
            return dynamic_cast<dummyNode*>( impl.get());
        }

        readNode* getReader(){
            return dynamic_cast<readNode*>( impl.get());
        }

        std::string getType(){
            return  nodeType;
        }

        MOCK_METHOD(void, execute, (), ());
};

// Sample fornode wrapper
// Checks executing time and adds a child with the constructor
class ForNodeMock :public ForNodeImpl{
    public:
        ForNodeMock(GameState* gameState,std::unique_ptr<childNode> t): ForNodeImpl("id", gameState){


            addChild(std::move(t));
        }

        childNode* getC(int index){
            return dynamic_cast<childNode*>( ForNodeImpl::children[index].get());
        }

        MOCK_METHOD(jsonReturnFormat, getJSON, (std::string id), ());
        MOCK_METHOD(void, execute, (), ());

};


// Temp commented out because we need to change the gameState to a pointer or a reference

TEST (RuleTests, forNodeWriteTest){
    // Setting up a vector labeled 1,2,3,4......
    int vecSize = 6;
    auto vecData = std::vector<int>(vecSize);
    std::iota(vecData.begin(), vecData.end(),0);
    // Reversing data to ensure results are correct
    std::reverse(vecData.begin(), vecData.end());
    // Get the total sum of 1+2+3+4....+(n-1)+n
    auto sum = std::accumulate(vecData.begin(), vecData.end(), 0);
    sum += vecSize;

    // Passing in dummy data
    json j;
    j[ARRAY_ID] = vecData;

    // Passing in the dummy indexes which will be incremented by the forNode
    json identifiers;
    identifiers[ARRAY_ID] = vecData;
    json indexes;
    indexes[ARRAY_ID] =0;

    GameState gs{&j};

    // Test if two childs are being executed
    std::string type = "child1";
    auto writer = std::make_unique<writeNode> (&gs, indexes, identifiers);
    auto child = std::make_unique<childNode>(type, "child1",&gs, std::move(writer));
    ASSERT_EQ(child->getType(), type);

    type = "child2";
    auto reader = std::make_unique<readNode> (&gs, indexes, identifiers);
    auto child2 = std::make_unique<childNode>(type, "child2",&gs, std::move(reader));
    ASSERT_EQ(child2->getType(), type);

    ForNodeMock fNode(&gs, std::move(child));
    fNode.addChild(std::move(child2));
    fNode.setIdentifierData(j);

    // Triggers the actual function call
    // Also checks it triggers once
    EXPECT_CALL(fNode, execute()).Times(1).WillOnce([&fNode]{
        return fNode.ForNodeImpl::execute();
    });

    fNode.execute();

    // Ensures that the dummy node incremeted its index vecSize times
    // Should be the case since the forNode will execute for every element in the dummy data
    // Dummy Node will sum up all elements in the vector so the counter is checking that
    auto counter = fNode.getC(1)->getReader()->getCounter();
    auto index = fNode.getC(1)->getReader()->getIndex();
    ASSERT_EQ(counter, sum);
    ASSERT_EQ(index, vecSize);


}

TEST (RuleTests, forNodeTwoChild){
    // Setting up a vector labeled 1,2,3,4......
    int vecSize = 6;
    auto vecData = std::vector<int>(vecSize);
    std::iota(vecData.begin(), vecData.end(),0);
    // Reversing data to ensure results are correct
    std::reverse(vecData.begin(), vecData.end());
    // Get the total sum of 1+2+3+4....+(n-1)+n
    auto sum = std::accumulate(vecData.begin(), vecData.end(), 0);

    // Passing in dummy data
    json j;
    j[ARRAY_ID] = vecData;

    // Passing in the dummy indexes which will be incremented by the forNode
    json identifiers;
    identifiers[ARRAY_ID] = vecData;
    json indexes;
    indexes[ARRAY_ID] =0;

    GameState gs{&j};

    // Test if two childs are being executed
    std::string type = "child1";
    auto dummy = std::make_unique<dummyNode> (&gs, indexes);
    dummy->setIdentifierData(identifiers);
    auto child = std::make_unique<childNode>(type, "child1",&gs, std::move(dummy));
    ASSERT_EQ(child->getType(), type);

    type = "child2";
    auto dummy2 = std::make_unique<dummyNode> (&gs, indexes);
    dummy2->setIdentifierData(identifiers);
    auto child2 = std::make_unique<childNode>(type, "child2",&gs, std::move(dummy2));
    ASSERT_EQ(child2->getType(), type);

    ForNodeMock fNode(&gs, std::move(child));
    fNode.addChild(std::move(child2));
    fNode.setIdentifierData(j);

    // Triggers the actual function call
    // Also checks it triggers once
    EXPECT_CALL(fNode, execute()).Times(1).WillOnce([&fNode]{
        return fNode.ForNodeImpl::execute();
    });

    fNode.execute();

    // Ensures that the dummy node incremeted its index vecSize times
    // Should be the case since the forNode will execute for every element in the dummy data
    // Dummy Node will sum up all elements in the vector so the counter is checking that
    auto counter = fNode.getC(0)->getImpl()->getCounter();
    auto index = fNode.getC(0)->getImpl()->getIndex();
    ASSERT_EQ(counter, sum);
    ASSERT_EQ(index, vecSize);

    counter = fNode.getC(1)->getImpl()->getCounter();
    index = fNode.getC(1)->getImpl()->getIndex();
    ASSERT_EQ(counter, sum);
    ASSERT_EQ(index, vecSize);
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

