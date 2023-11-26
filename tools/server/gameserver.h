
#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "gameParser.h"
#include "MessageQueue.h"
#include "Server.h"
#include <vector>

class GameServer {
public:
    GameServer(unsigned short port, const std::string& htmlResponse,
               void (*onConnectCallback)(networking::Connection),
               void (*onDisconnectCallback)(networking::Connection));

    void update();

    std::deque<networking::Message> receive();

    void send(const std::deque<networking::Message>& messages);

    void disconnect(networking::Connection connection);

    networking::Server& getServer();

    std::string getMessage();

private:
    networking::Server server;
    SendMessageQueue in;
    ReceiveMessageQueue out;
    TreeNode root;
};

#endif //GAMESERVER_H
