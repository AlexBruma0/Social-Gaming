#include <gtest/gtest.h>
#include <fstream>
#include <chrono>
#include "Server.h"
#include "gameServer.h"

#include "ruleNode.h"
#include "ruleNodeSupport.h"

#include "utils.h"
#include "GameState.h"

// Wrapper API
#include <cpp-tree-sitter.h>

// Custom Library in lib
#include "parser.h"

// Game Parser functionality
#include "gameParser.h"

class childNode: public TreeNode{
    public:
        childNode(const ts::Node& node, std::string type, std::string& sourceCode, GameState* gameState, std::unique_ptr<TreeNodeImpl> d){
            impl = std::move(d);
        }

        void changeType(std::string type){
            nodeType = type;
        }

        std::string getType(){
            return  nodeType;
        }
};

void
onConnect(networking::Connection c) {
    std::cout << "New connection found: " << c.id << "\n";
}


void
onDisconnect(networking::Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
}

std::string
getHTTPMessage(const char* htmlLocation) {
    if (access(htmlLocation, R_OK ) != -1) {
        std::ifstream infile{htmlLocation};
        return std::string{std::istreambuf_iterator<char>(infile),
                           std::istreambuf_iterator<char>()};

    }

    std::cerr << "Unable to open HTML index file:\n"
              << htmlLocation << "\n";
    std::exit(-1);
}

GameServer getServer(SendMessageQueue* in, ReceiveMessageQueue* out, GameState* gs){
    int port = 8000;
    const char *argv2;
    argv2 = "resources/networking/webchat.html";
    GameServer gameServer(port, getHTTPMessage(argv2), in, out, gs);
    return gameServer;
}

ts::Node getEmptyTSNode(){
    std::string noStringSourcecode = "";
    ts::Tree noStringTree = string_to_tree(noStringSourcecode);

    ts::Node noStringNode = noStringTree.getRootNode();
    return noStringNode;
}

TEST(ServerTests, treeNodeCalls){
    int vecSize = 6;
    auto vecData = std::vector<int>(vecSize);
    std::iota(vecData.begin(), vecData.end(),0);
    std::vector<int> weapons = {1, 2, 3};
    json j;
    j["players"] = vecData;
    j[TreeNodeImpl::TIMEOUT_ID] = "10";
    j["weapons"] = weapons;
    j["player"] = 1;
    j["prompt"] = "choose your weapon!";

    // Passing in the dummy indexes which will be incremented by the forNode
    json identifiers;
    identifiers[TreeNodeImpl::COLLECTION_ID] = "players";
    identifiers[TreeNodeImpl::VARIABLE_ID] = "player";

    json identifiersInput;
    identifiersInput[TreeNodeImpl::CHOICES_ID] = "weapons";
    identifiersInput[TreeNodeImpl::TARGET_ID] = "player";
    identifiersInput[TreeNodeImpl::PROMPT_ID] = "prompt"; 
    identifiersInput[TreeNodeImpl::TIMEOUT_ID] = "10";

    GameVariables idVars;
    createGameVariables(identifiers, idVars);

    GameVariables gameVars;
    createGameVariables(j, gameVars);

    GameVariables inputVars;
    createGameVariables(identifiersInput, inputVars);

    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer g = getServer(&in, &out, &gs);

    gs.setVars(gameVars);

    std::string type = "child";
    auto parallelNode = std::make_unique<ParallelForNodeImpl> (type, &gs, &in, &out);
    parallelNode->setNodeVariables(idVars);

    type = "input_choice";
    auto input = std::make_unique<InputChoiceNodeImpl> (type, &gs, &in, &out);
    ts::Node tsNode = getEmptyTSNode();
    input->setNodeVariables(inputVars);
    

    //need to change type after to avoid parsing segfaults
    auto child = std::make_unique<childNode>(tsNode, type, type, &gs, std::move(input));
    child->changeType("input_choice");
    parallelNode->addChild(std::move(child));

    auto start_time = std::chrono::high_resolution_clock::now();
    parallelNode->execute();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    EXPECT_GE(duration.count(), 10);
}

TEST(GAMESERVER_TEST, timeout) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState g(nullptr, nullptr);
    GameServer gs = getServer(&in, &out, &g);

    auto start_time = std::chrono::high_resolution_clock::now();
    gs.awaitResponse(5, {});
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    EXPECT_GE(duration.count(), 5);
}

TEST(GAMESERVER_TEST, timeout_empty) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState g(nullptr, nullptr);
    GameServer gs = getServer(&in, &out, &g);

    auto start_time = std::chrono::high_resolution_clock::now();
    gs.awaitResponse(0, {});
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    EXPECT_LE(duration.count(), 1);
}