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

TEST(messageQueue_tests, simpleSendSizeTest){
    SendMessageQueue mq = SendMessageQueue();
    std::string prompt = "";
    networking::SendMessage msg{emptyChoices, prompt};
    mq.add(msg);
    mq.add(msg);
    mq.add(msg);
    ASSERT_EQ(mq.size(), 3);
}

TEST(messageQueue_tests, simpleReceiveSizeTest){
    ReceiveMessageQueue mq = ReceiveMessageQueue();
    int choice = 1;
    networking::Connection con{8};
    networking::ReceiveMessage msg{choice, con};
    mq.add(msg);
    mq.add(msg);
    mq.add(msg);
    ASSERT_EQ(mq.size(), 3);
}


TEST(messageQueue_tests, AddAndRetrieveMessage) {
    SendMessageQueue mq;
    networking::SendMessage msg1{emptyChoices, "Test Message 1"};
    mq.add(msg1);
    ASSERT_EQ(mq.remove().prompt, "Test Message 1");
}


TEST(messageQueue_tests, MultipleAddsAndSequentialRemove) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "First"));
    mq.add(networking::SendMessage(emptyChoices, "Second"));
    ASSERT_EQ(mq.remove().prompt, "First");
    ASSERT_EQ(mq.remove().prompt, "Second");
}

TEST(messageQueue_tests, RemoveFromQueuePreservesOrder) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "First in Line"));
    mq.add(networking::SendMessage(emptyChoices, "Second in Line"));
    ASSERT_EQ(mq.remove().prompt, "First in Line");
    ASSERT_EQ(mq.remove().prompt, "Second in Line");
}


TEST(messageQueue_tests, AddDuplicateMessages) {
    SendMessageQueue mq;
    networking::SendMessage msg{emptyChoices, "Duplicate"};
    mq.add(msg);
    mq.add(msg);
    ASSERT_EQ(mq.size(), 2);
}

TEST(messageQueue_tests, AddMessageWithLargePrompt) {
    SendMessageQueue mq;
    std::string largePrompt(1000, 'x'); // 1000 'x' characters
    mq.add(networking::SendMessage(emptyChoices, largePrompt));
    ASSERT_EQ(mq.size(), 1);
}

TEST(messageQueue_tests, AddMessageWithSpecialCharacters) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "Special!@#$%^&*()"));
    ASSERT_EQ(mq.size(), 1);
}

TEST(messageQueue_tests, CheckSizeAfterMultipleRemoves) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "Msg1"));
    mq.add(networking::SendMessage(emptyChoices, "Msg2"));
    mq.remove();
    mq.remove();
    ASSERT_EQ(mq.size(), 0);
}

//remove
// TEST(messageQueue_tests, RemoveFromQueueMultipleTimes) {
//     SendMessageQueue mq;
//     mq.add(networking::SendMessage(emptyChoices, "Only Message"));
//     mq.remove();
//     ASSERT_THROW(mq.remove(), std::out_of_range);
// }

TEST(messageQueue_tests, AddMessageWithEmptyChoices) {
    SendMessageQueue mq;
    std::vector<int> noChoices;
    mq.add(networking::SendMessage(noChoices, "No Choices Message"));
    ASSERT_EQ(mq.size(), 1);
}

TEST(messageQueue_tests, QueueSizeAfterRandomOperations) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "Add1"));
    mq.add(networking::SendMessage(emptyChoices, "Add2"));
    mq.remove();
    mq.add(networking::SendMessage(emptyChoices, "Add3"));
    ASSERT_EQ(mq.size(), 2);
}

//
//remove
// TEST(messageQueue_tests, AddNullChoicesMessage) {
//     SendMessageQueue mq;
//     std::vector<int> *nullChoices = nullptr;
//     ASSERT_THROW(mq.add(networking::SendMessage(*nullChoices, "Null Choices")), std::invalid_argument);
// }


TEST(messageQueue_tests, RemoveMessageAfterMultipleAdds) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "Msg1"));
    mq.add(networking::SendMessage(emptyChoices, "Msg2"));
    mq.remove();
    ASSERT_EQ(mq.remove().prompt, "Msg2");
}

TEST(messageQueue_tests, AddMessageWithLargeDataSet) {
    SendMessageQueue mq;
    std::vector<int> largeChoices(1000, 1); // 1000 choices
    mq.add(networking::SendMessage(largeChoices, "Large Data Set"));
    ASSERT_EQ(mq.size(), 1);
}

TEST(messageQueue_tests, RemoveMessageWithLargeDataSet) {
    SendMessageQueue mq;
    std::vector<int> largeChoices(1000, 1); // 1000 choices
    mq.add(networking::SendMessage(largeChoices, "Large Data Set"));
    ASSERT_NO_THROW(mq.remove());
}

// TEST(messageQueue_tests, QueueEmptyAfterMultipleOperations) {
//     SendMessageQueue mq;
//     mq.add(networking::SendMessage(emptyChoices, "Op1"));
//     mq.remove();
//     mq.add(networking::SendMessage(emptyChoices, "Op2"));
//     mq.remove();
//     ASSERT_TRUE(mq.empty());
// }

TEST(messageQueue_tests, CheckOrderAfterRandomAddsAndRemoves) {
    SendMessageQueue mq;
    mq.add(networking::SendMessage(emptyChoices, "First"));
    mq.add(networking::SendMessage(emptyChoices, "Second"));
    mq.remove();
    mq.add(networking::SendMessage(emptyChoices, "Third"));
    ASSERT_EQ(mq.remove().prompt, "Second");
    ASSERT_EQ(mq.remove().prompt, "Third");
}


//more receive
TEST(messageQueue_tests, AddMessageWithExtremeChoiceValues) {
    ReceiveMessageQueue mq;
    mq.add(networking::ReceiveMessage(INT_MAX, networking::Connection(123)));
    mq.add(networking::ReceiveMessage(INT_MIN, networking::Connection(456)));
    ASSERT_EQ(mq.size(), 2);
}

TEST(messageQueue_tests, AddAndCheckSize) {
    ReceiveMessageQueue mq;
    mq.add(networking::ReceiveMessage(1, networking::Connection(1111)));
    ASSERT_EQ(mq.size(), 1);
}

//remove
// TEST(messageQueue_tests, RemoveMessageNotPresent) {
//     ReceiveMessageQueue mq;
//     ASSERT_THROW(mq.remove(), std::out_of_range);
// }

TEST(messageQueue_tests, GetMessageWithValidID) {
    ReceiveMessageQueue mq;
    networking::Connection con(2222);
    mq.add(networking::ReceiveMessage(2, con));
    auto messages = mq.getMessageFromID(con);
    ASSERT_FALSE(messages.empty());
}

// TEST(messageQueue_tests, GetMessageWithInvalidID) {
//     ReceiveMessageQueue mq;
//     networking::Connection con(3333);
//     mq.add(networking::ReceiveMessage(3, con));
//     auto messages = mq.getMessageFromID(networking::Connection(4444));
// }

// TEST(messageQueue_tests, RemoveAndCheckEmpty) {
//     ReceiveMessageQueue mq;
//     mq.add(networking::ReceiveMessage(4, networking::Connection(5555)));
//     mq.remove();
// }

TEST(messageQueue_tests, AddMultipleAndCheckOrder) {
    ReceiveMessageQueue mq;
    mq.add(networking::ReceiveMessage(5, networking::Connection(6666)));
    mq.add(networking::ReceiveMessage(6, networking::Connection(7777)));
    ASSERT_EQ(mq.remove().connection.id, 6666);
    ASSERT_EQ(mq.remove().connection.id, 7777);
}

TEST(messageQueue_tests, QueueCapacityTest) {
    ReceiveMessageQueue mq;
    for (int i = 0; i < 100; ++i) {
        mq.add(networking::ReceiveMessage(i, networking::Connection(i)));
    }
    ASSERT_EQ(mq.size(), 100);
}

// TEST(messageQueue_tests, ClearQueueMultipleMessages) {
//     ReceiveMessageQueue mq;
//     mq.add(networking::ReceiveMessage(7, networking::Connection(8888)));
//     mq.add(networking::ReceiveMessage(8, networking::Connection(9999)));
//     mq.clear();
// }

TEST(messageQueue_tests, RetrieveAllMessagesForConnection) {
    ReceiveMessageQueue mq;
    networking::Connection con(12345);
    mq.add(networking::ReceiveMessage(9, con));
    mq.add(networking::ReceiveMessage(10, con));
    auto messages = mq.getMessageFromID(con);
    ASSERT_EQ(messages.size(), 2);
}

TEST(messageQueue_tests, CheckMessageIntegrityAfterRemoval) {
    ReceiveMessageQueue mq;
    networking::Connection con(54321);
    mq.add(networking::ReceiveMessage(11, con));
    auto removedMsg = mq.remove();
    ASSERT_EQ(removedMsg.choice, 11);
    ASSERT_EQ(removedMsg.connection.id, con.id);
}

// TEST(messageQueue_tests, EmptyQueueAfterMultipleRemovals) {
//     ReceiveMessageQueue mq;
//     mq.add(networking::ReceiveMessage(12, networking::Connection(13579)));
//     mq.add(networking::ReceiveMessage(13, networking::Connection(24680)));
//     mq.remove();
//     mq.remove();
// }

// TEST(messageQueue_tests, RemoveFromQueueAfterClear) {
//     ReceiveMessageQueue mq;
//     mq.add(networking::ReceiveMessage(14, networking::Connection(112233)));
//     mq.clear();
//     ASSERT_THROW(mq.remove(), std::out_of_range);
// }