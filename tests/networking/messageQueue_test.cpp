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