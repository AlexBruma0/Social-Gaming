#include <vector>
#include <string>
#include <string_view>
#include "Server.h"

class MessageQueue{
    private:
    std::vector<networking::Message2> messages;
    public:
    MessageQueue& operator=(const MessageQueue& mq);
    int add(networking::Message2 message);
    networking::Message2 remove();
    std::vector<networking::Message2> getMessageFromID(networking::Connection);
};
