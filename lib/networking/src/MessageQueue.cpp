
#include "MessageQueue.h"

MessageQueue & MessageQueue::operator=(const MessageQueue &mq){
    this->messages = mq.messages;
}
void MessageQueue::add(std::string s){
    messages.push_back(s);
}
std::string MessageQueue::remove(){
    std::string message = messages.front();
    messages.erase(messages.begin());
    return message;
}