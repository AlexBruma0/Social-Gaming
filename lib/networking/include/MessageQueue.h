#include <vector>
#include <string>
#include <string_view>
#include "Server.h"

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

class MessageQueue{
    private:
    std::vector<networking::Message> messages;
    public:
    MessageQueue& operator=(const MessageQueue& mq);
    int add(networking::Message message);
    networking::Message remove();
    std::vector<networking::Message> getMessageFromID(networking::Connection);
};

#endif // MESSAGE_QUEUE_H