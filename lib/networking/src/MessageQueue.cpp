
#include "MessageQueue.h"

MessageQueue & MessageQueue::operator=(const MessageQueue &mq){
    this->messages = mq.messages;
    return *this;
}

int MessageQueue::add(std::string s){
    messages.push_back(s);
    return messages.size()-1;
}

std::string MessageQueue::remove(){
    std::string message = messages.front();
    messages.erase(messages.begin());
    return message;
}

std::string MessageQueue::getMessageFromID(int id){
    if(id>=messages.size() || id < 0){
        return "";
    }
    return messages[id];
}