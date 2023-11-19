#include <gtest/gtest.h>
#include "MessageQueue.h"

TEST(messageQueue_tests, simpletest){
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    mq.add(input);
    mq.add(input2);
    ASSERT_EQ(mq.remove(),input);
    ASSERT_EQ(mq.remove(),input2);
}

TEST(messageQueue_tests, idTest){
    MessageQueue mq = MessageQueue();
    std::string input = "input";
    std::string input2 = "input2";
    int id1 = mq.add(input);
    int id2 = mq.add(input2);
    ASSERT_EQ(mq.getMessageFromID(id1), input);
    ASSERT_EQ(mq.getMessageFromID(id2), input2);
}