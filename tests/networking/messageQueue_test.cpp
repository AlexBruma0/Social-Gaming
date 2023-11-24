#include <gtest/gtest.h>
#include "MessageQueue.h"

std::vector<std::string> emptyChoices;

TEST(messageQueue_tests, simpletest)
{
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    networking::Connection con1{8888};
    networking::Message2 msg1{networking::MessageType::SEND, emptyChoices, input, con1};
    networking::Connection con2{8888};
    networking::Message2 msg2{networking::MessageType::SEND, emptyChoices, input2, con2};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_EQ(mq.remove().prompt,input);
    ASSERT_EQ(mq.remove().prompt,input2);
}

TEST(messageQueue_tests, idTest){
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    networking::Connection con1{8888};
    networking::Message2 msg1{networking::MessageType::SEND, emptyChoices, input, con1};
    networking::Connection con2{9999};
    networking::Message2 msg2{networking::MessageType::SEND, emptyChoices, input2, con2};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_EQ(mq.getMessageFromID(con1)[0].prompt, input);
    ASSERT_EQ(mq.getMessageFromID(con2)[0].prompt, input2);
}

TEST(messageQueue_tests, idNotFoundTest){
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    networking::Connection con1{8888};
    networking::Message2 msg1{networking::MessageType::SEND, emptyChoices, input, con1};
    networking::Connection con2{9999};
    networking::Message2 msg2{networking::MessageType::SEND, emptyChoices, input2, con2};
    networking::Connection falseConnection{7777};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_TRUE(mq.getMessageFromID(falseConnection).empty());
    ASSERT_FALSE(mq.getMessageFromID(con2).empty());
}

TEST(messageQueue_tests, realisticUseTest){
    MessageQueue mq = MessageQueue();

    //Server generates message to be put into Queue
    std::string prompt = "Select Weapon: 1, 2, 3";
    networking::Connection clientConnection{1};
    networking::Message2 outgoingMessage {networking::MessageType::SEND, emptyChoices, prompt, clientConnection};

    //add Message to the Queue
    mq.add(outgoingMessage);

    //Server pulls the oldest message from the queue to send
    networking::Message2 toSendMessage = mq.remove();
    //Server can now send to client by using their connectionID: toSendMessage.connection
    ASSERT_EQ(toSendMessage.connection.id, clientConnection.id);
    //Client can display prompt: toSendMessage.prompt
    ASSERT_EQ(toSendMessage.prompt, prompt);
    //List of choices not necessary I believe for send message
    ASSERT_TRUE(toSendMessage.choices.empty());
}