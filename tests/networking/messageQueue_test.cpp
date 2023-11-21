#include <gtest/gtest.h>
#include "MessageQueue.h"

TEST(messageQueue_tests, simpletest){
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    networking::Connection con1{8888};
    networking::Message msg1{con1, input};
    networking::Connection con2{8888};
    networking::Message msg2{con1, input2};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_EQ(mq.remove().text,input);
    ASSERT_EQ(mq.remove().text,input2);
}

TEST(messageQueue_tests, idTest){
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    networking::Connection con1{8888};
    networking::Message msg1{con1, input};
    networking::Connection con2{9999};
    networking::Message msg2{con2, input2};
    mq.add(msg1);
    mq.add(msg2);
    ASSERT_EQ(mq.getMessageFromID(con1)[0].text, input);
    ASSERT_EQ(mq.getMessageFromID(con2)[0].text, input2);
}