#include <gtest/gtest.h>
#include "MessageQueue.h"

std::vector<int> emptyChoices;

TEST(messageQueue_tests, simpleSendTest)
{
    SendMessageQueue mq = SendMessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    networking::SendMessage msg1{emptyChoices, input};
    networking::SendMessage msg2{emptyChoices, input2};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_EQ(mq.remove().prompt, input);
    ASSERT_EQ(mq.remove().prompt, input2);
}

TEST(messageQueue_tests, simpleReceiveTest){
    ReceiveMessageQueue mq = ReceiveMessageQueue();
    int choice = 1;
    int choice2 = 2;
    networking::Connection con1{8888};
    networking::ReceiveMessage msg1{choice, con1};
    networking::Connection con2{9999};
    networking::ReceiveMessage msg2{choice2, con2};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_EQ(mq.getMessageFromID(con1)[0].choice, choice);
    ASSERT_EQ(mq.getMessageFromID(con2)[0].choice, choice2);
}

TEST(messageQueue_tests, idNotFoundTest){
    ReceiveMessageQueue mq = ReceiveMessageQueue();
    int choice = 1;
    int choice2 = 2;
    networking::Connection con1{8888};
    networking::ReceiveMessage msg1{choice, con1};
    networking::Connection con2{9999};
    networking::ReceiveMessage msg2{choice2, con2};
    networking::Connection falseConnection{7777};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_TRUE(mq.getMessageFromID(falseConnection).empty());
    ASSERT_FALSE(mq.getMessageFromID(con2).empty());
}

TEST(messageQueue_tests, realisticUseTest){
    SendMessageQueue smq = SendMessageQueue();

    //Server generates message to be put into Queue
    std::string prompt = "Select Weapon: 1, 2, 3";
    std::vector<int> weaponChoices{1, 2, 3};
    networking::SendMessage outgoingMessage {weaponChoices, prompt};

    //add Message to the Queue
    smq.add(outgoingMessage);

    networking::SendMessage sendToClient = smq.remove();
    // here we would broadcast the prompt to the clients

    ReceiveMessageQueue rmq = ReceiveMessageQueue();
    networking::Connection con1{7777};
    networking::Connection con2{8888};
    networking::Connection con3{9999};

    // after the server gets responses from the client, push responses to the queue
    networking::ReceiveMessage rm1 {2, con1};
    networking::ReceiveMessage rm2 {1, con2};
    networking::ReceiveMessage rm3 {5, con3};
    rmq.add(rm1);
    rmq.add(rm2);
    rmq.add(rm3);

    // the game logic retrieves from the queue
    networking::ReceiveMessage received1 = rmq.remove();
    ASSERT_EQ(received1.connection.id, con1.id);

    networking::ReceiveMessage received2 = rmq.remove();
    ASSERT_EQ(received2.connection.id, con2.id);

    networking::ReceiveMessage received3 = rmq.remove();
    ASSERT_EQ(received3.connection.id, con3.id);
    ASSERT_FALSE(std::find(weaponChoices.begin(), weaponChoices.end(), received3.choice) != weaponChoices.end());
}