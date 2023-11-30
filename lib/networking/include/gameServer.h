#ifndef GAMESERVER_H
#define GAMESERVER_H

#pragma once

#include "MessageQueue.h"
#include "Server.h"
#include "ruleNode.h"
#include <vector>

class GameServer {
public:
    GameServer(unsigned short port, const std::string& htmlResponse, SendMessageQueue* in, ReceiveMessageQueue* out);

    void update();

    std::deque<networking::Message> receive();

    void send(const std::deque<networking::Message>& messages);

    void disconnect(networking::Connection connection);

    networking::Server& getServer();

    std::string getMessage();

    void broadcastMessage();

    void awaitResponse(int timeout, const std::vector<int>& choices);

    void printClients() {
        std::cout << "clients: \n";
        for (auto client : clients) {
            std::cout << "client: " << client.id << std::endl;
        }
    }

    std::deque<networking::Message> buildOutgoing(const std::string& log);

private:
    networking::Server server;
    SendMessageQueue* in;
    ReceiveMessageQueue* out;
    TreeNode root;
    std::vector<networking::Connection> clients;

    void processResponses(const std::deque<networking::Message>& messages, std::vector<int> choices);

    void onConnectCallback(networking::Connection c);
    void onDisconnectCallback(networking::Connection c);
};

#endif //GAMESERVER_H
