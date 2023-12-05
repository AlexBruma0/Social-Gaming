#include <unistd.h>
#include "gameServer.h"
#include "Server.h"
#include "gameParser.h"

#define RPS_LOCATION "resources/games/rock-paper-scissors.game"

GameServer::GameServer(unsigned short port, const std::string& htmlResponse, SendMessageQueue* in, ReceiveMessageQueue* out, GameState* gs)
        : server(port, htmlResponse,
                 [this](networking::Connection c) { onConnectCallback(c); },
                 [this](networking::Connection c) { onDisconnectCallback(c); }),
          in(in),
          out(out) {

    // initialize the root of the rules
    std::string sourcecode = file_to_string(RPS_LOCATION);
    ts::Tree tree = string_to_tree(sourcecode);
    ts::Node tsRoot = tree.getRootNode();

    root = buildRuleTree(tsRoot, sourcecode, in, out, this, gs);
}

void GameServer::update() {
    server.update();
}

std::deque<networking::Message> GameServer::receive() {
    return server.receive();
}

void GameServer::send(const std::deque<networking::Message>& messages) {
    server.send(messages);
    update();
}

void GameServer::disconnect(networking::Connection connection) {
    server.disconnect(connection);
}

networking::Server& GameServer::getServer() {
    return server;
}

std::string GameServer::getMessage() {
    // dummy string for now, just to make sure we can get data from the game
    return root.getType();
}

void GameServer::sendNextMessage(networking::Connection con){
    networking::SendMessage sm = in->remove();
    server.sendSingle(sm, con);
    
}


void GameServer::broadcastMessage() {
    // take a message from the in queue
    networking::SendMessage sm = in->remove();
    std::string prompt = sm.prompt;
    // build the messages to each client
    const auto outgoing = buildOutgoing(prompt);
    send(outgoing);
}

void GameServer::awaitResponse(int timeout, const std::vector<int>& choices) {
    int elapsedTime = 0;
    // for [timeout] seconds, await responses
    while (true) {
        // get any incoming messages and put the result, if valid, into the outgoing queue
        update();
        const auto incoming = receive();
        // for (auto& m : incoming){
        //     std::cout<< m.text<<std::endl;
        // }
    
        processResponses(incoming, choices);

        if (elapsedTime > timeout) {
            return;
        }

        elapsedTime++;
        sleep(1);
    }
}

void GameServer::processResponses(const std::deque<networking::Message>& messages, std::vector<int> choices) {
    for (const auto& message : messages) {
        try {
            int response = std::stoi(message.text);
            auto it = std::find(choices.begin(), choices.end(), response);

            if (it != choices.end()) {
                // if the response is one of the valid choices, then add it to the outgoing queue
                auto rm = networking::ReceiveMessage {response, message.connection};
                out->add(rm);

            } else {
                // do nothing for now, but once we can actually send messages to each client individually,
                // inform them that they've entered some invalid choice (out of range)
            }
        } catch (const std::invalid_argument& e) {
            // do nothing for now, but once we can actually send messages to each client individually,
            // inform them that they've entered some invalid choice (not numerical)
        }
    }
}

std::deque<networking::Message> GameServer::buildOutgoing(const std::string& log) {
    std::deque<networking::Message> outgoing;
    for (auto client : clients) {
        outgoing.push_back({client, log});
    }
    return outgoing;
}

void GameServer::onConnectCallback(networking::Connection c) {
    std::cout << "New connection found: " << c.id << "\n";
    clients.push_back(c);
    //playerOrder.push_back(c); // Add the player to the order queue
}


void GameServer::onDisconnectCallback(networking::Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
    auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
    clients.erase(eraseBegin, std::end(clients));
}
