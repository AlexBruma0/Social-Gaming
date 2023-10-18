#include "Server.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>


using networking::Server;
using networking::Connection;
using networking::Message;


// //placeholder for game struct, will change to just invite code maybe
// struct GameInfo {
//     std::string owner;
//     // shared 4-digit invite code for the game
//     std::string sharedInviteCode; 
// };

// // vector for all game instances
// std::vector<GameInfo> gameDatabase;

// // generates new 4 digit code from 1000-9999
// std::string inviteGeneration() {
//     std::random_device rd;
//     std::mt19937 mt(rd());
//     std::uniform_int_distribution<int> dist(1000, 9999);

//     int inviteCodeNumber = dist(mt);
//     return std::to_string(inviteCodeNumber);
// }

// //placeholder code for game class (I think LEx is working on that)
// std::string create(const std::string& owner) {
//     std::string inviteCode = inviteGeneration();

//     GameInfo newGame;
//     newGame.owner = owner;
//     newGame.inviteCode = inviteCode;
//     gameDatabase.push_back(newGame);
    
//     return inviteCode;
// }

// // somewhat placeholder for when user joins after takiing in input, will call from MessageResult
// bool join(const std::string& inviteCode, const std::string& username) {
//     for (const auto& game : gameDatabase) {
//         if (game.inviteCode == inviteCode) {
//             std::string owner = game.owner;

//             std::cout << "User " << username << " joined the game owned by '" << owner << "'.\n";
//             return true;
//         }
//     }

//     std::cout << "Game with invite code '" << sharedInviteCode << "' not found.\n";
//     return false;
// }

std::vector<Connection> clients;
//for multiple sessions
// std::vector<Server> gameServers;

void
onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  clients.push_back(c);
}


void
onDisconnect(Connection c) {
  std::cout << "Connection lost: " << c.id << "\n";
  auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
  clients.erase(eraseBegin, std::end(clients));
}


struct MessageResult {
  std::string result;
  bool shouldShutdown;
};


MessageResult
processMessages(Server& server, const std::deque<Message>& incoming) {
  std::ostringstream result;
  bool quit = false;
  for (const auto& message : incoming) {
    if (message.text == "quit") {
      server.disconnect(message.connection);
    } else if (message.text == "shutdown") {
      std::cout << "Shutting down.\n";
      quit = true;
    } else {
      result << message.connection.id << "> " << message.text << "\n";
    }
  }
  return MessageResult{result.str(), quit};
}


std::deque<Message>
buildOutgoing(const std::string& log) {
  std::deque<Message> outgoing;
  for (auto client : clients) {
    outgoing.push_back({client, log});
  }
  return outgoing;
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
void StartGameServer(unsigned short port, const char* htmlLocation) {
  Server server{port, getHTTPMessage(htmlLocation), onConnect, onDisconnect};
  // gameServers.push_back(server);
}

int
main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
              << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
    return 1;
  }

  const unsigned short port = std::stoi(argv[1]);
  Server server{port, getHTTPMessage(argv[2]), onConnect, onDisconnect};

  //parse with for loop, create new
  for (int i = 1; i < argc; i += 2) {
    unsigned short port = std::stoi(argv[i]);
    const char* htmlLocation = argv[i + 1];
    StartGameServer(port, htmlLocation);
  }

  while (true) {
    bool errorWhileUpdating = false;
    try {
      server.update();
    } catch (std::exception& e) {
      std::cerr << "Exception from Server update:\n"
                << " " << e.what() << "\n\n";
      errorWhileUpdating = true;
    }

    const auto incoming = server.receive();
    const auto [log, shouldQuit] = processMessages(server, incoming);
    const auto outgoing = buildOutgoing(log);
    server.send(outgoing);

    if (shouldQuit || errorWhileUpdating) {
      break;
    }

    sleep(1);
  }

  return 0;
}