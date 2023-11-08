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


std::vector<Connection> clients;
std::vector<std::string> codes;

namespace networking {

// Forward declaration
struct Connection;

// Player class
class Player {
public:
    Player(Connection conn) : connection(conn) {}

    // Getters and setters for player information
    Connection getConnection() const { return connection; }
    // Additional player-related methods can be added here

private:
    Connection connection;
    // Additional player-related data members can be added here
};

// Game class
class Game {
public:
    Game(std::string gameID, std::string gameFile) 
        : id(gameID), file(gameFile) {}

    // Function to add a player to the game
    void addPlayer(const Player& player) {
        players.push_back(player);
    }

    // Getters and setters for game information
    std::string getID() const { return id; }
    std::string getFile() const { return file; }
    const std::vector<Player>& getPlayers() const { return players; }
    // Additional game-related methods can be added here

private:
    std::string id;
    std::string file;
    std::vector<Player> players;
    // Additional game-related data members can be added here
};

} // namespace networking

std::vector<networking::Game> games;
std::unordered_map<std::string, networking::Game*> gameCodeMap;

std::string generateFourDigitNumber() {
    // random num generator seed
    static bool seedInitialized = false;
    if (!seedInitialized) {
      // uses current time as seed 
        std::srand(time(nullptr)); 
        seedInitialized = true;
    }

    // Random number between 1000 and 9999
    int randomNumber = 1000 + std::rand() % 9000; 
    return std::to_string(randomNumber);
}

void handleCreateRequest(Server& server, const Message &message, std::deque<Message>& responseMessages) {
    std::string code = generateFourDigitNumber();
    codes.push_back(code);
    
    // Create a new game with a unique ID and game file (placeholder names)
    networking::Game newGame(code, "path_to_game_file");
    games.push_back(newGame);
    gameCodeMap[code] = &games.back();

    std::cout << "Created game with code: " << code << "\n";
    std::string response = "Game created. Your code is: " + code;
    responseMessages.push_back({message.connection, response});
    server.send(responseMessages);
}

void handleJoinRequest(Server& server, const Message &message, std::deque<Message>& responseMessages) {
    std::string code = /* extract code from message.text */;
    
    auto it = gameCodeMap.find(code);
    if (it != gameCodeMap.end()) {
        // Add player to the game
        networking::Player player(message.connection);
        it->second->addPlayer(player);

        std::string response = "Joined game with code: " + code;
        responseMessages.push_back({message.connection, response});
    } else {
        std::string response = "There is no game with code: " + code;
        responseMessages.push_back({message.connection, response});
    }
    server.send(responseMessages);
}


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
  //created deque of response messages
  std::deque<Message> responseMessages;
  for (const auto& message : incoming) {
    if (message.text == "q") {
      server.disconnect(message.connection);
    } else if (message.text == "shutdown") {
      std::cout << "Shutting down.\n";
      quit = true;
      
    } else if (message.text == "create_game") {
        handleCreateRequest(server, message, responseMessages);

    } else if (message.text == "join_game") {
        handleJoinRequest(server, message, responseMessages);
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


int
main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
              << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
    return 1;
  }

  const unsigned short port = std::stoi(argv[1]);
  Server server{port, getHTTPMessage(argv[2]), onConnect, onDisconnect};

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