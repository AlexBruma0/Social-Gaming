
#include <gtest/gtest.h>
#include <deque>
#include "gameserver.cpp"  // Assuming gameserver.cpp includes all necessary dependencies

// Test fixture for gameserver tests
class GameServerTest : public ::testing::Test {
protected:
    // Setup code common to all tests can be added here
    void SetUp() override {
        // Initialization code, if any
    }

    // Tear-down code common to all tests can be added here
    void TearDown() override {
        // Cleanup code, if any
    }
};

// Test cases for generateFourDigitNumber function
TEST_F(GameServerTest, GenerateFourDigitNumberRange) {
    for (int i = 0; i < 100; ++i) {
        std::string number = generateFourDigitNumber();
        int num = std::stoi(number);
        EXPECT_GE(num, 1000);
        EXPECT_LE(num, 9999);
    }
}

// Mocking Server and Message objects may be necessary for the following tests
// Test cases for handleCreateRequest function
TEST_F(GameServerTest, HandleCreateRequestBehavior) {
    // Mock objects and their behavior
    // Server server;  // Assuming Server is a mockable object
    // Message message;  // Assuming Message is a mockable object
    std::deque<Message> responseMessages;

    // Call the function
    // handleCreateRequest(server, message, responseMessages);

    // Check if codes vector is updated correctly
    // EXPECT_FALSE(codes.empty());

    // Check if the response message is constructed correctly
    // ASSERT_FALSE(responseMessages.empty());
    // EXPECT_EQ(responseMessages.front().text, "Game created. Your code is: " + codes.back());
}

TEST_F(GameServerTest, HandleJoinRequestValid) {
    Server mockServer;
    Connection mockConnection;
    std::deque<Message> responseMessages;
    Message joinMessage{mockConnection, "join_game"};
    handleJoinRequest(mockServer, joinMessage, responseMessages);

    // Verify player added to game
    EXPECT_EQ(games.front().getPlayers().size(), 1);

    // Verify response message
    EXPECT_FALSE(responseMessages.empty());
    EXPECT_EQ(responseMessages.front().text, "Joined game with code: 1111");
}


TEST_F(GameServerTest, HandleJoinRequestInvalid) {
    Server mockServer;
    Connection mockConnection;
    std::deque<Message> responseMessages;
    Message joinMessage{mockConnection, "join_game"};
    handleJoinRequest(mockServer, joinMessage, responseMessages);

    // Verify no player added to game
    EXPECT_TRUE(games.empty() || games.front().getPlayers().empty());

    // Verify response message
    EXPECT_FALSE(responseMessages.empty());
    EXPECT_EQ(responseMessages.front().text, "There is no game with code: 1111");
}

TEST_F(GameServerTest, HandleGameChoiceRequest) {
    Server mockServer;
    Connection mockConnection;
    std::deque<Message> responseMessages;
    Message choiceMessage{mockConnection, "game_choice"};
    playerOrder.push_back(mockConnection); // Simulate a player in queue

    handleGameChoiceRequest(mockServer, choiceMessage, responseMessages);

    // Verify player removed from queue
    EXPECT_TRUE(playerOrder.empty());

    // Verify response message
    EXPECT_FALSE(responseMessages.empty());
    EXPECT_EQ(responseMessages.front().text, "You have chosen a game.");
}

TEST_F(GameServerTest, OnConnect) {
    Connection newConnection;
    onConnect(newConnection);

    // Verify connection added
    EXPECT_FALSE(clients.empty());
    EXPECT_FALSE(playerOrder.empty());
    EXPECT_EQ(clients.back(), newConnection);
    EXPECT_EQ(playerOrder.back(), newConnection);
}

TEST_F(GameServerTest, OnDisconnect) {
    Connection lostConnection;
    clients.push_back(lostConnection); // Simulate existing connection
    onDisconnect(lostConnection);

    // Verify connection removed
    EXPECT_TRUE(clients.empty());
}

TEST_F(GameServerTest, ProcessMessagesCommandHandling) {
    Server mockServer;
    std::deque<Message> incomingMessages;
    incomingMessages.push_back({Connection(), "create_game"});

    auto result = processMessages(mockServer, incomingMessages);

    // Verify appropriate action taken
    EXPECT_FALSE(games.empty());
    EXPECT_EQ(result.result, "");
    EXPECT_FALSE(result.shouldShutdown);
}


TEST_F(GameServerTest, BuildOutgoingMessages) {
    Connection client1, client2;
    clients.push_back(client1);
    clients.push_back(client2);
    std::string log = "Test Log";

    auto outgoing = buildOutgoing(log);

    // Verify outgoing messages for each client
    EXPECT_EQ(outgoing.size(), clients.size());
    for (const auto& message : outgoing) {
        EXPECT_EQ(message.text, log);
    }
}

// The following two are cases we should plan for in the future. 

TEST_F(GameServerTest, HandleClientReconnection) { 
    /*
    Server mockServer;
    Connection mockConnection;
    onConnect(mockConnection); // Simulate a connection
    onDisconnect(mockConnection); // Simulate disconnection
    onConnect(mockConnection); // Simulate reconnection
    */
    // Verify the client is properly handled on reconnection
    // This might include checking if the client's state is restored
    // EXPECT_TRUE(/* check client's state or presence in the server */);
}

TEST_F(GameServerTest, HandleInvalidMessages) {
    /*
    Server mockServer;
    Connection mockConnection;
    std::deque<Message> responseMessages;
    Message invalidMessage{mockConnection, "invalid content"};

    handleInvalidRequest(mockServer, invalidMessage, responseMessages);
    */
    // Verify appropriate response or error handling for invalid messages
    // EXPECT_TRUE(/* check response or error handling logic */);
}
