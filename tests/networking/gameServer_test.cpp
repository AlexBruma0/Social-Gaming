#include <gtest/gtest.h>
#include <fstream>
#include "Server.h"
#include "gameserver.h"

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

TEST(dummy, dummy) {
    int port = 8000;
    const char *argv2;
    argv2 = "resources/networking/webchat.html";

    GameServer gameServer(port, getHTTPMessage(argv2));
    SUCCEED();
}