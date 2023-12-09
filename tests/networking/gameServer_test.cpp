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
    testMessage.connection.id = 1;
    testMessage.text = "Test message";
    std::deque<networking::Message> messages;
    messages.push_back(testMessage);
    
    server.send(messages);
    // Update the server to process the message
    server.update();

    // Receive messages from the server
    std::deque<networking::Message> receivedMessages = server.receive();

    // Assuming that you expect to receive the same message that was sent
    ASSERT_FALSE(receivedMessages.empty());
    const networking::Message& received = receivedMessages.front();

    EXPECT_EQ(received.connection.id, testMessage.connection.id);
    EXPECT_EQ(received.text, testMessage.text);
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

    // Create a message
    networking::Message testMessage;
    testMessage.connection.id = 1; 
    testMessage.text = "Hello";    

    // Add the message to a deque
    std::deque<networking::Message> messages;
    messages.push_back(testMessage);

    // Send the deque of messages
    server.send(messages);

    // Assuming 'in' queue will have the message after send operation
    auto sentMessage = in.remove(); 

    EXPECT_EQ(sentMessage.prompt, testMessage.text);
}


TEST(GAMESERVER_TEST, HandleMultipleClients) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate multiple clients sending messages
    for (int i = 1; i <= 5; i++) {
        networking::Message msg;
        msg.connection.id = i;
        msg.text = "Client " + std::to_string(i);
        std::deque<networking::Message> messages;
        messages.push_back(msg);
        server.send(messages); // Send each message individually
    }

    // Update the server to process the messages
    server.update();

    // Receive messages from the server
    std::deque<networking::Message> receivedMessages = server.receive();

    // Check if messages from all clients are received
    ASSERT_EQ(receivedMessages.size(), 5); // Expecting 5 messages
    for (int i = 0; i < 5; i++) {
        const networking::Message& received = receivedMessages[i];
        EXPECT_EQ(received.connection.id, i + 1);
        EXPECT_EQ(received.text, "Client " + std::to_string(i + 1));
    }
}


TEST(GAMESERVER_TEST, InvalidMessageHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Create an invalid message
    networking::Message invalidMessage;
    invalidMessage.connection.id = -1;  // Set an invalid connection ID
    invalidMessage.text = "Invalid";   

    // Prepare a deque with the invalid message
    std::deque<networking::Message> messages;
    messages.push_back(invalidMessage);

    // Send the invalid message
    server.send(messages);

    // Update the server to process the message
    server.update();

    // Check the server's response to the invalid message
    // Since 'in' is the sending queue and 'out' is the receiving queue,
    // we expect 'out' to be empty if the server correctly handles invalid messages
    EXPECT_TRUE(out.size() == 0);
}


TEST(GAMESERVER_TEST, ServerShutdown) { /* We should make shutdown method first
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    
    server.shutdown();

    // Check if the server is no longer accepting messages
    networking::Message testMessage;
    testMessage.connectionId = 1;
    testMessage.data = "Test after shutdown";
    in.push(testMessage);

    server.update();

    // The message should not be processed
    EXPECT_TRUE(out.size() == 0); */
}

TEST(GAMESERVER_TEST, EmptyMessageHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Create an empty message
    networking::Message emptyMessage;
    emptyMessage.connection.id = 1; // Assuming a valid connection ID
    emptyMessage.text = "";        // Assuming 'text' is the correct field for message content

    // Prepare a deque with the empty message
    std::deque<networking::Message> messages;
    messages.push_back(emptyMessage);

    // Send the empty message
    server.send(messages);

    // Update the server to process the message
    server.update();

    // Check how the server handles empty messages
    // Assuming server.receive() will get the processed messages
    std::deque<networking::Message> receivedMessages = server.receive();

    // Check if the server processed the empty message correctly
    ASSERT_FALSE(receivedMessages.empty());
    const networking::Message& received = receivedMessages.front();
    EXPECT_EQ(received.connection.id, emptyMessage.connection.id);
    EXPECT_EQ(received.text, emptyMessage.text);
}


TEST(GAMESERVER_TEST, HandleDisconnectedClients) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate a client connecting and then disconnecting
    networking::Message connectMessage;
    connectMessage.connection.id = 1;
    connectMessage.text = "Client connected";
    
    std::deque<networking::Message> messages;
    messages.push_back(connectMessage);
    server.send(messages);
    server.update();

    // Simulate disconnection
    server.disconnect(connectMessage.connection);

    server.update();
    server.printClients(); // Should be empty. 
}

TEST(GAMESERVER_TEST, BroadcastMessage) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);

    // Simulate multiple clients
    for (int i = 1; i <= 3; i++) {
        networking::SendMessage msg;
        msg.prompt = "Client " + std::to_string(i);
        in.add(msg);
    }
    server.update();

    // Broadcast a message to all clients
    server.broadcastMessage();

    // Check if all clients received the broadcast message
    for (int i = 1; i <= 3; i++) {
        auto received = in.remove();
        EXPECT_EQ(received.prompt, "Broadcast test");
    }
}

TEST(GAMESERVER_TEST, CorruptedMessageHandling) {
    /*
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
    */
}

TEST(GAMESERVER_TEST, LoadHandling) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);
    std::vector<int> emptyChoices;

    // Simulate high load by sending a large number of messages
    for (int i = 0; i < 1000; i++) {
        std::string msg = "Message " + std::to_string(i);
        in.add(networking::SendMessage(emptyChoices, msg));
    }

    server.update();

    // Check if server can handle high load without crashing or losing messages
    for (int i = 0; i < 1000; i++) {
        EXPECT_FALSE(out.size() == 0);
        out.remove();
    }
}

TEST(GAMESERVER_TEST, ResponseLatency) {
    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameState gs(nullptr, nullptr);
    GameServer server = getServer(&in, &out, &gs);
    std::vector<int> emptyChoices;

    in.add(networking::SendMessage(emptyChoices, "Latency Test"));

    auto start_time = std::chrono::high_resolution_clock::now();
    server.update();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // Check the response latency is within an acceptable range (e.g., less than 100ms)
    EXPECT_LE(latency, 100);
}
