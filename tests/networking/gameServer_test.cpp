#include <gtest/gtest.h>
#include <fstream>
#include <chrono>
#include "Server.h"
#include "gameServer.h"

std::string getHTTPMessage(const char* htmlLocation) {
    if (access(htmlLocation, R_OK ) != -1) {
        std::ifstream infile{htmlLocation};
        return std::string{std::istreambuf_iterator<char>(infile),
                           std::istreambuf_iterator<char>()};

    }

    std::cerr << "Unable to open HTML index file:\n"
              << htmlLocation << "\n";
    std::exit(-1);
}

TEST(GAMESERVER_TEST, timeout) {
    int port = 8000;
    const char *argv2;
    argv2 = "resources/networking/webchat.html";

    GameServer gs(port, getHTTPMessage(argv2));

    auto start_time = std::chrono::high_resolution_clock::now();
    gs.awaitResponse(5, {});
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    EXPECT_GE(duration.count(), 5);
}

TEST(GAMESERVER_TEST, timeout_empty) {
    int port = 8000;
    const char *argv2;
    argv2 = "resources/networking/webchat.html";

    GameServer gs(port, getHTTPMessage(argv2));

    auto start_time = std::chrono::high_resolution_clock::now();
    gs.awaitResponse(0, {});
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    EXPECT_LE(duration.count(), 1);
}
