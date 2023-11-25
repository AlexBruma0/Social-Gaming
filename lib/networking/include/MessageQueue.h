#include <vector>
#include <string>
#include <string_view>
#include "Server.h"

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

class SendMessageQueue{
    private:
    std::vector<networking::SendMessage> messages;
    public:
    SendMessageQueue& operator=(const SendMessageQueue& mq);
    int add(networking::SendMessage message);
    networking::SendMessage remove();
};

class ReceiveMessageQueue{
private:
    std::vector<networking::ReceiveMessage> messages;
public:
    ReceiveMessageQueue& operator=(const ReceiveMessageQueue& mq);
    int add(networking::ReceiveMessage message);
    networking::ReceiveMessage remove();
    std::vector<networking::ReceiveMessage> getMessageFromID(networking::Connection);
};

#endif // MESSAGE_QUEUE_H