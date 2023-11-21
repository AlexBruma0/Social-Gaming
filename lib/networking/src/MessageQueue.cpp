
#include "MessageQueue.h"

MessageQueue & MessageQueue::operator=(const MessageQueue &mq){
    this->messages = mq.messages;
    return *this;
}

int MessageQueue::add(networking::Message message){
    messages.push_back(message);
    return messages.size()-1;
}

networking::Message MessageQueue::remove(){
    networking::Message message = messages.front();
    messages.erase(messages.begin());
    return message;
}

std::vector<networking::Message> MessageQueue::getMessageFromID(networking::Connection connection){
    uintptr_t id = connection.id;
    std::vector<networking::Message> returnVec;
    for(networking::Message msg:messages){
        if(msg.connection.id == id) returnVec.push_back(msg);
    }
    return returnVec;
}