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
        dummyNode(GameState* gameState, json& indexes, json& identifiers): TreeNodeImpl("dummy", gameState){
            idIndexes = indexes;
            setIdentifierData(identifiers);
        }
        void execute(){
            auto idData = getIdentifierData();
            testCounter += idData[ARRAY_ID][getIndex()].get<size_t>();
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
        childNode(std::string node, std::string type, GameState* gameState, std::unique_ptr<TreeNodeImpl> d): TreeNode(node, type, gameState){
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
        ForNodeMock(GameState* gameState):ForNodeImpl("id", gameState){}

        childNode* getChild(int index){
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
    auto counter = fNode.getChild(1)->getReader()->getCounter();
    auto index = fNode.getChild(1)->getReader()->getIndex();
    ASSERT_EQ(counter, sum);
    ASSERT_EQ(index, vecSize);


}

TEST(RuleTests, multiChildReadWriteTest){
    //Data Setup


    std::vector<int> dummyVec = {1,2,3,4,5};
    std::reverse(dummyVec.begin(), dummyVec.end());
    auto sum = std::accumulate(dummyVec.begin(), dummyVec.end(), 0);


    json j;
    j[ARRAY_ID] = dummyVec;
    json identifiers;
    identifiers[ARRAY_ID] = dummyVec;
    json indexes;
    indexes[ARRAY_ID] = 0;

    GameState gs{&j};

    //Node Setup

    auto wdummy1 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto wdummy2 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto wdummy3 = std::make_unique<writeNode>(&gs, indexes, identifiers);

    auto rdummy1 = std::make_unique<readNode>(&gs, indexes, identifiers);
    auto rdummy2 = std::make_unique<readNode>(&gs, indexes, identifiers);
    auto rdummy3 = std::make_unique<readNode>(&gs, indexes, identifiers);

    std::unique_ptr<childNode> wcNode1 = std::make_unique<childNode>("child1", "child1", &gs, std::move(wdummy1));
    std::unique_ptr<childNode> wcNode2 = std::make_unique<childNode>("child2", "child2", &gs, std::move(wdummy2));
    std::unique_ptr<childNode> wcNode3 = std::make_unique<childNode>("child3", "child3", &gs, std::move(wdummy3));

    std::unique_ptr<childNode> rcNode1 = std::make_unique<childNode>("child4", "child4", &gs, std::move(rdummy1));
    std::unique_ptr<childNode> rcNode2 = std::make_unique<childNode>("child5", "child5", &gs, std::move(rdummy2));
    std::unique_ptr<childNode> rcNode3 = std::make_unique<childNode>("child6", "child6", &gs, std::move(rdummy3));

    ForNodeMock parentNode(&gs, std::move(wcNode1));
    parentNode.setIdentifierData(j);

    // //ASSERT that nodes have been properly setup
    ASSERT_EQ(parentNode.getChild(0)->getType(), "child1");
    ASSERT_EQ(wcNode2->getType(), "child2");
    ASSERT_EQ(wcNode3->getType(), "child3");
    ASSERT_EQ(rcNode1->getType(), "child4");
    ASSERT_EQ(rcNode2->getType(), "child5");
    ASSERT_EQ(rcNode3->getType(), "child6");

    //attempt adding children to parentNode

    parentNode.addChild(std::move(rcNode1));
    parentNode.addChild(std::move(wcNode2));
    parentNode.addChild(std::move(rcNode2));
    parentNode.addChild(std::move(wcNode3));
    parentNode.addChild(std::move(rcNode3));

    //Execute for loop
    EXPECT_CALL(parentNode, execute()).Times(1).WillOnce([&parentNode]
        { return parentNode.ForNodeImpl::execute(); 
        }
    );
    parentNode.execute();

    //get each dummy node sum

    auto cVal1 = parentNode.getChild(1)->getReader()->getCounter();
    auto cVal2 = parentNode.getChild(3)->getReader()->getCounter();
    auto cVal3 = parentNode.getChild(5)->getReader()->getCounter();

    //3 writes so i*arrsize
    int vecSize = dummyVec.size();
    //Verfy each Node value is equal to SUM + Write values
    ASSERT_EQ(sum+vecSize, cVal1);
    ASSERT_EQ(sum+2*vecSize, cVal2);
    ASSERT_EQ(sum+3*vecSize, cVal3);
    // get indexs of for loop to check behaviour
    auto index1 = parentNode.getChild(1)->getReader()->getIndex();
    auto index2 = parentNode.getChild(3)->getReader()->getIndex();
    auto index3 = parentNode.getChild(5)->getReader()->getIndex();

    ASSERT_EQ(index1, vecSize);
    ASSERT_EQ(index2, vecSize);
    ASSERT_EQ(index3, vecSize);
}

TEST(RuleTests, multiChildDummyTest){
    // Data Setup

    std::vector<int> dummyVec = {1, 2, 3, 4, 5};
    int vecSize = dummyVec.size();
    std::reverse(dummyVec.begin(), dummyVec.end());
    auto sum = std::accumulate(dummyVec.begin(), dummyVec.end(), 0);

    json j;
    j[ARRAY_ID] = dummyVec;
    json identifiers;
    identifiers[ARRAY_ID] = dummyVec;
    json indexes;
    indexes[ARRAY_ID] = 0;

    GameState gs{&j};

    // Node Setup

    auto dummy1 = std::make_unique<dummyNode>(&gs, indexes, identifiers);
    auto dummy2 = std::make_unique<dummyNode>(&gs, indexes, identifiers);
    auto dummy3 = std::make_unique<dummyNode>(&gs, indexes, identifiers);

    dummy1->setIdentifierData(identifiers);
    dummy2->setIdentifierData(identifiers);
    dummy3->setIdentifierData(identifiers);

    std::unique_ptr<childNode> cNode1 = std::make_unique<childNode>("child1", "child1", &gs, std::move(dummy1));
    std::unique_ptr<childNode> cNode2 = std::make_unique<childNode>("child2", "child2", &gs, std::move(dummy2));
    std::unique_ptr<childNode> cNode3 = std::make_unique<childNode>("child3", "child3", &gs, std::move(dummy3));
    

    ForNodeMock parentNode(&gs, std::move(cNode1));
    parentNode.setIdentifierData(j);


    parentNode.addChild(std::move(cNode2));
    parentNode.addChild(std::move(cNode3));

    // Execute for loop
    EXPECT_CALL(parentNode, execute()).Times(1).WillOnce([&parentNode]
        { return parentNode.ForNodeImpl::execute(); });
    parentNode.execute();

    // get each dummy node sum

    auto cVal1 = parentNode.getChild(0)->getImpl()->getCounter();
    auto cVal2 = parentNode.getChild(1)->getImpl()->getCounter();
    auto cVal3 = parentNode.getChild(2)->getImpl()->getCounter();

    ASSERT_EQ(cVal1, sum);
    ASSERT_EQ(cVal2, sum);
    ASSERT_EQ(cVal3, sum);

    //get indexs of for loop to check behaviour
    auto index1 = parentNode.getChild(0)->getImpl()->getIndex();
    auto index2 = parentNode.getChild(1)->getImpl()->getIndex();
    auto index3 = parentNode.getChild(2)->getImpl()->getIndex();

    ASSERT_EQ(index1, vecSize);
    ASSERT_EQ(index2, vecSize);
    ASSERT_EQ(index3, vecSize);
}

TEST(RuleTests, multiTypeNodeTest){
    // Data Setup

    std::vector<int> dummyVec = {1, 2, 3, 4, 5};
    int vecSize = dummyVec.size();
    std::reverse(dummyVec.begin(), dummyVec.end());
    auto sum = std::accumulate(dummyVec.begin(), dummyVec.end(), 0);

    json j;
    j[ARRAY_ID] = dummyVec;
    json identifiers;
    identifiers[ARRAY_ID] = dummyVec;
    json indexes;
    indexes[ARRAY_ID] = 0;

    GameState gs{&j};

    //Child Node Setup
    auto dummy1 = std::make_unique<dummyNode>(&gs, indexes, identifiers);
    auto wdummy1 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto wdummy2 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto rdummy1 = std::make_unique<readNode>(&gs, indexes, identifiers);
    auto rdummy2 = std::make_unique<readNode>(&gs, indexes, identifiers);

    std::unique_ptr<childNode> cNode1 = std::make_unique<childNode>("child1", "child1", &gs, std::move(dummy1));
    std::unique_ptr<childNode> wcNode1 = std::make_unique<childNode>("child2", "child2", &gs, std::move(wdummy1));
    std::unique_ptr<childNode> wcNode2 = std::make_unique<childNode>("child3", "child3", &gs, std::move(wdummy2));

    std::unique_ptr<childNode> rcNode1 = std::make_unique<childNode>("child4", "child4", &gs, std::move(rdummy1));
    std::unique_ptr<childNode> rcNode2 = std::make_unique<childNode>("child5", "child5", &gs, std::move(rdummy2));

    //Parent Node Setup
    ForNodeMock parentNode(&gs);
    parentNode.setIdentifierData(j);
    parentNode.addChild(std::move(wcNode1));
    parentNode.addChild(std::move(rcNode1));
    parentNode.addChild(std::move(cNode1));
    parentNode.addChild(std::move(wcNode2));
    parentNode.addChild(std::move(rcNode2));

    // Execute for loop
    EXPECT_CALL(parentNode, execute()).Times(1).WillOnce([&parentNode]
        { return parentNode.ForNodeImpl::execute(); });
    parentNode.execute();

    // get each dummy node value

    auto cVal1 = parentNode.getChild(2)->getImpl()->getCounter();
    auto rcVal1 = parentNode.getChild(1)->getReader()->getCounter();
    auto rcVal2 = parentNode.getChild(4)->getReader()->getCounter();

    ASSERT_EQ(cVal1, sum);
    ASSERT_EQ(rcVal1, sum+vecSize);
    ASSERT_EQ(rcVal2, sum+2*vecSize);

    // get indexs of for loop to check behaviour
    auto index1 = parentNode.getChild(2)->getImpl()->getIndex();
    auto index2 = parentNode.getChild(1)->getReader()->getIndex();
    auto index3 = parentNode.getChild(4)->getReader()->getIndex();

    ASSERT_EQ(index1, vecSize);
    ASSERT_EQ(index2, vecSize);
    ASSERT_EQ(index3, vecSize);
}

TEST (RuleTests, emptyDataTest){
    // Data Setup

    std::vector<int> dummyVec = {};
    int vecSize = dummyVec.size();
    std::reverse(dummyVec.begin(), dummyVec.end());
    auto sum = 0;

    json j;
    j[ARRAY_ID] = dummyVec;
    json identifiers;
    identifiers[ARRAY_ID] = dummyVec;
    json indexes;
    indexes[ARRAY_ID] = 0;

    GameState gs{&j};

    // Child Node Setup
    auto dummy1 = std::make_unique<dummyNode>(&gs, indexes, identifiers);
    auto wdummy1 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto wdummy2 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto rdummy1 = std::make_unique<readNode>(&gs, indexes, identifiers);
    auto rdummy2 = std::make_unique<readNode>(&gs, indexes, identifiers);

    std::unique_ptr<childNode> cNode1 = std::make_unique<childNode>("child1", "child1", &gs, std::move(dummy1));
    std::unique_ptr<childNode> wcNode1 = std::make_unique<childNode>("child2", "child2", &gs, std::move(wdummy1));
    std::unique_ptr<childNode> wcNode2 = std::make_unique<childNode>("child3", "child3", &gs, std::move(wdummy2));

    std::unique_ptr<childNode> rcNode1 = std::make_unique<childNode>("child4", "child4", &gs, std::move(rdummy1));
    std::unique_ptr<childNode> rcNode2 = std::make_unique<childNode>("child5", "child5", &gs, std::move(rdummy2));

    // Parent Node Setup
    ForNodeMock parentNode(&gs);
    parentNode.setIdentifierData(j);
    parentNode.addChild(std::move(wcNode1));
    parentNode.addChild(std::move(rcNode1));
    parentNode.addChild(std::move(cNode1));
    parentNode.addChild(std::move(wcNode2));
    parentNode.addChild(std::move(rcNode2));

    // Execute for loop
    EXPECT_CALL(parentNode, execute()).Times(1).WillOnce([&parentNode]
        { return parentNode.ForNodeImpl::execute(); });
    parentNode.execute();

    // get each dummy node value

    auto cVal1 = parentNode.getChild(2)->getImpl()->getCounter();
    auto rcVal1 = parentNode.getChild(1)->getReader()->getCounter();
    auto rcVal2 = parentNode.getChild(4)->getReader()->getCounter();

    //Make sure each value is equal to 0

    ASSERT_EQ(cVal1, sum);
    ASSERT_EQ(rcVal1, sum);
    ASSERT_EQ(rcVal2, sum);

    // get indexs of for loop to check behaviour
    auto index1 = parentNode.getChild(2)->getImpl()->getIndex();
    auto index2 = parentNode.getChild(1)->getReader()->getIndex();
    auto index3 = parentNode.getChild(4)->getReader()->getIndex();

    //make sure each index remains at 0

    ASSERT_EQ(index1, vecSize);
    ASSERT_EQ(index2, vecSize);
    ASSERT_EQ(index3, vecSize);
}

TEST(RuleTests, ConsecutiveWriteTest){
    // Data Setup

    std::vector<int> dummyVec = {1, 1, 1};
    int vecSize = dummyVec.size();
    std::reverse(dummyVec.begin(), dummyVec.end());
    auto sum = std::accumulate(dummyVec.begin(), dummyVec.end(), 0);

    json j;
    j[ARRAY_ID] = dummyVec;
    json identifiers;
    identifiers[ARRAY_ID] = dummyVec;
    json indexes;
    indexes[ARRAY_ID] = 0;

    GameState gs{&j};

    // Child Node Setup
    auto wdummy1 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto wdummy2 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto rdummy1 = std::make_unique<readNode>(&gs, indexes, identifiers);
    auto wdummy3 = std::make_unique<writeNode>(&gs, indexes, identifiers);
    auto dummy1 = std::make_unique<dummyNode>(&gs, indexes, identifiers);

    std::unique_ptr<childNode> wcNode1 = std::make_unique<childNode>("child2", "child2", &gs, std::move(wdummy1));
    std::unique_ptr<childNode> wcNode2 = std::make_unique<childNode>("child3", "child3", &gs, std::move(wdummy2));
    std::unique_ptr<childNode> wcNode3 = std::make_unique<childNode>("child5", "child5", &gs, std::move(wdummy3));

    std::unique_ptr<childNode> rcNode1 = std::make_unique<childNode>("child4", "child4", &gs, std::move(rdummy1));
    std::unique_ptr<childNode> cNode1 = std::make_unique<childNode>("child1", "child1", &gs, std::move(dummy1));

    // Parent Node Setup
    ForNodeMock parentNode(&gs);
    parentNode.setIdentifierData(j);
    parentNode.addChild(std::move(wcNode1));
    parentNode.addChild(std::move(wcNode2));
    parentNode.addChild(std::move(wcNode3));
    parentNode.addChild(std::move(cNode1));
    parentNode.addChild(std::move(rcNode1));

    // Execute for loop
    EXPECT_CALL(parentNode, execute()).Times(1).WillOnce([&parentNode]
        { return parentNode.ForNodeImpl::execute(); });
    parentNode.execute();

    // get each dummy node value

    auto cVal1 = parentNode.getChild(3)->getImpl()->getCounter();
    auto rcVal1 = parentNode.getChild(4)->getReader()->getCounter();

    // Make sure each value is equal to the sum plus 3 writes
    int writes = vecSize;

    ASSERT_EQ(cVal1, vecSize);
    ASSERT_EQ(rcVal1, sum + 3*writes);

    // get indexs of for loop to check behaviour
    auto index1 = parentNode.getChild(3)->getImpl()->getIndex();
    auto index2 = parentNode.getChild(4)->getReader()->getIndex();

    // make sure each index remains at 0

    ASSERT_EQ(index1, vecSize);
    ASSERT_EQ(index2, vecSize);
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
    auto dummy = std::make_unique<dummyNode> (&gs, indexes, identifiers);
    dummy->setIdentifierData(identifiers);
    auto child = std::make_unique<childNode>(type, "child1",&gs, std::move(dummy));
    ASSERT_EQ(child->getType(), type);

    type = "child2";
    auto dummy2 = std::make_unique<dummyNode> (&gs, indexes, identifiers);
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
    auto counter = fNode.getChild(0)->getImpl()->getCounter();
    auto index = fNode.getChild(0)->getImpl()->getIndex();
    ASSERT_EQ(counter, sum);
    ASSERT_EQ(index, vecSize);

    counter = fNode.getChild(1)->getImpl()->getCounter();
    index = fNode.getChild(1)->getImpl()->getIndex();
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

