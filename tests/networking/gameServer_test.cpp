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

TEST(GAMESERVER_TEST, MessageHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    networking::Message testMessage;
    testMessage.connectionId = 1;
    testMessage.data = "Test message";
    in.push(testMessage);

    server.update();
    auto received = out.pop();

    EXPECT_EQ(received.connectionId, testMessage.connectionId);
    EXPECT_EQ(received.data, testMessage.data);
}

TEST(GAMESERVER_TEST, TimeoutHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    auto start_time = std::chrono::high_resolution_clock::now();
    server.awaitResponse(10, {});
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    EXPECT_GE(duration.count(), 10);
}

TEST(GAMESERVER_TEST, SendMessage) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    std::string message = "Hello";
    server.sendMessage(1, message);

    auto sentMessage = in.pop();
    EXPECT_EQ(sentMessage.data, message);
}

TEST(GAMESERVER_TEST, HandleMultipleClients) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate multiple clients sending messages
    for (int i = 1; i <= 5; i++) {
        networking::Message msg;
        msg.connectionId = i;
        msg.data = "Client " + std::to_string(i);
        in.push(msg);
    }

    server.update();

    // Check if messages from all clients are received
    for (int i = 1; i <= 5; i++) {
        auto received = out.pop();
        EXPECT_EQ(received.connectionId, i);
        EXPECT_EQ(received.data, "Client " + std::to_string(i));
    }
}

TEST(GAMESERVER_TEST, InvalidMessageHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    networking::Message invalidMessage;
    invalidMessage.connectionId = -1;
    invalidMessage.data = "Invalid";
    in.push(invalidMessage);

    server.update();

    // Expect the server not to crash or behave unexpectedly
    EXPECT_TRUE(out.empty());
}

TEST(GAMESERVER_TEST, ServerShutdown) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Call the shutdown method (you need to implement it if it doesn't exist)
    server.shutdown();

    // Check if the server is no longer accepting messages
    networking::Message testMessage;
    testMessage.connectionId = 1;
    testMessage.data = "Test after shutdown";
    in.push(testMessage);

    server.update();

    // The message should not be processed
    EXPECT_TRUE(out.empty());
}

TEST(GAMESERVER_TEST, EmptyMessageHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    networking::Message emptyMessage;
    emptyMessage.connectionId = 1;
    emptyMessage.data = "";
    in.push(emptyMessage);

    server.update();

    // Check how the server handles empty messages
    auto received = out.pop();
    EXPECT_EQ(received.connectionId, 1);
    EXPECT_EQ(received.data, "");
}

TEST(GAMESERVER_TEST, HandleDisconnectedClients) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate a client connecting and then disconnecting
    networking::Message connectMessage;
    connectMessage.connectionId = 1;
    connectMessage.data = "Client connected";
    in.push(connectMessage);
    server.update();

    // Simulate disconnection
    server.handleClientDisconnection(1);

    // Attempt to send a message to the disconnected client
    server.sendMessage(1, "Test message to disconnected client");
    server.update();

    // Expect that no message is sent to the disconnected client
    EXPECT_TRUE(out.empty());
}

TEST(GAMESERVER_TEST, BroadcastMessage) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate multiple clients
    for (int i = 1; i <= 3; i++) {
        networking::Message msg;
        msg.connectionId = i;
        msg.data = "Client " + std::to_string(i);
        in.push(msg);
    }
    server.update();

    // Broadcast a message to all clients
    server.broadcastMessage("Broadcast test");

    // Check if all clients received the broadcast message
    for (int i = 1; i <= 3; i++) {
        auto received = out.pop();
        EXPECT_EQ(received.data, "Broadcast test");
    }
}

TEST(GAMESERVER_TEST, CorruptedMessageHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate a corrupted message (e.g., malformed data)
    networking::Message corruptedMessage;
    corruptedMessage.connectionId = 1;
    corruptedMessage.data = "\x01\x02\x03\x04"; // Non-ASCII characters
    in.push(corruptedMessage);

    server.update();

    // Check how the server handles corrupted messages
    EXPECT_TRUE(out.empty()); // Expect no response or handled gracefully
}

TEST(GAMESERVER_TEST, LoadHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate high load by sending a large number of messages
    for (int i = 0; i < 1000; i++) {
        networking::Message msg;
        msg.connectionId = 1;
        msg.data = "Message " + std::to_string(i);
        in.push(msg);
    }

    server.update();

    // Check if server can handle high load without crashing or losing messages
    for (int i = 0; i < 1000; i++) {
        EXPECT_FALSE(out.empty());
        out.pop();
    }
}

TEST(GAMESERVER_TEST, ResponseLatency) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    networking::Message testMessage;
    testMessage.connectionId = 1;
    testMessage.data = "Latency test";
    in.push(testMessage);

    auto start_time = std::chrono::high_resolution_clock::now();
    server.update();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // Check the response latency is within an acceptable range (e.g., less than 100ms)
    EXPECT_LE(latency, 100);
}
