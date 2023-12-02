
#include "MessageQueue.h"

SendMessageQueue & SendMessageQueue::operator=(const SendMessageQueue &mq){
    this->messages = mq.messages;
    return *this;
}

int SendMessageQueue::add(networking::SendMessage message) const {
    messages.push_back(message);
    return messages.size()-1;
}

networking::SendMessage SendMessageQueue::remove(){
    if(size() > 0){
        networking::SendMessage message = messages.front();
        messages.erase(messages.begin());
        return message;
    }

    std::vector<int> c;
    return networking::SendMessage{c, networking::SendMessage::INVALID};
    
}

ReceiveMessageQueue & ReceiveMessageQueue::operator=(const ReceiveMessageQueue &mq){
    this->messages = mq.messages;
    return *this;
}

int ReceiveMessageQueue::add(networking::ReceiveMessage message){
    messages.push_back(message);
    return messages.size()-1;
}

networking::ReceiveMessage ReceiveMessageQueue::remove(){
    if(size() > 0){
        networking::ReceiveMessage message = messages.front();
        messages.erase(messages.begin());
        return message;
    }
    networking::Connection c;
    return networking::ReceiveMessage{networking::ReceiveMessage::INVALID, c};
}

std::vector<networking::ReceiveMessage> ReceiveMessageQueue::getMessageFromID(networking::Connection connection){
    uintptr_t id = connection.id;
    std::vector<networking::ReceiveMessage> returnVec;
    for(networking::ReceiveMessage msg:messages){
        if(msg.connection.id == id) returnVec.push_back(msg);
    }
    return returnVec;
}

int SendMessageQueue::size(){
    return this->messages.size();
}

int ReceiveMessageQueue::size(){
    return this->messages.size();
}
