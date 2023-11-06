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
      std::cout << "Created game with code: " << code << "\n";
      // Construct a response message
      std::string response = "Game created. Your code is: " + code;
      responseMessages.push_back({message.connection, response});
      // Send the deque of response messages back to the client
      server.send(responseMessages);
      // result << message.connection.id << "> " << response << "\n";
}

void handleJoinRequest(Server& server, const Message &message, std::deque<Message>& responseMessages) {
      std::cout << "Attempting to join game with such code: \n";
      //find a game with such a game code id

      for(std::string c : codes) {
        if()
      }
      // Construct a response message
      std::string response = "There is no game with code: ";
      responseMessages.push_back({message.connection, response});
      // Send the deque of response messages back to the client
      server.send(responseMessages);
      // result << message.connection.id << "> " << response << "\n";
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