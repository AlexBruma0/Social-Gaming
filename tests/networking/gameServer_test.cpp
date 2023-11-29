#include <gtest/gtest.h>
#include <fstream>
#include "Server.h"
#include "gameServer.h"

#include "ruleNode.h"
#include "ruleNodeSupport.h"

#include "utils.h"
#include "GameState.h"

void
onConnect(networking::Connection c) {
    std::cout << "New connection found: " << c.id << "\n";
}


void
onDisconnect(networking::Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
}

std::string
getHTTPMessage(const char* htmlLocation) {
    if (access(htmlLocation, R_OK ) != -1) {
        std::ifstream infile{htmlLocation};
        return std::string{std::istreambuf_iterator<char>(infile),
                           std::istreambuf_iterator<char>()};

    }

    std::cerr << "Unable to open HTML index file:\n"
              << htmlLocation << "\n";
    std::exit(-1);
}

GameServer getServer(SendMessageQueue* in, ReceiveMessageQueue* out){
    int port = 8000;
    const char *argv2;
    argv2 = "resources/networking/webchat.html";

    GameServer gameServer(port, getHTTPMessage(argv2), in, out);
    return gameServer;
}


TEST(ServerTests, treeNodeCalls){
    int vecSize = 6;
    auto vecData = std::vector<int>(vecSize);
    std::iota(vecData.begin(), vecData.end(),0);
    std::reverse(vecData.begin(), vecData.end());
    auto sum = std::accumulate(vecData.begin(), vecData.end(), 0);
    sum += vecSize;

    json j;
    j["players"] = vecData;
    j[TreeNodeImpl::TIMEOUT_ID] = "10";

    // Passing in the dummy indexes which will be incremented by the forNode
    json identifiers;
    identifiers[TreeNodeImpl::COLLECTION_ID] = "players";
    identifiers[TreeNodeImpl::VARIABLE_ID] = "player";
    identifiers[TreeNodeImpl::TIMEOUT_ID] = "10";

    GameVariables idVars;
    createGameVariables(identifiers, idVars);

    GameVariables gameVars;
    createGameVariables(j, gameVars);

    SendMessageQueue in = SendMessageQueue();
    ReceiveMessageQueue out = ReceiveMessageQueue();
    GameServer g = getServer(&in, &out);
    GameState gs{&j, &g};
    gs.setVars(gameVars);

    std::string type = "child";
    auto assignment = std::make_unique<ParallelForNodeImpl> (type, &gs, &in, &out);
    assignment->setNodeVariables(idVars);
    assignment->execute();
}