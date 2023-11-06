#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Client.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;


//callback struct to create actions for the buttons
struct ButtonHandler {
    bool& done;
    networking::Client& client;
    std::string& entry;

    void operator()() {}
};

enum class Page {
  MainMenu,
  JoinGame,
  CreateGame,
  HelpMenu
};

void RunChatClient(networking::Client& client);

void HandleButtonClick(Page& page, networking::Client& client, const std::string& buttonLabel);

int main(int argc, char* argv[]) {
  if (argc < 3) {
      std::cerr << "Usage: \n  " << argv[0] << " <ip address> <port>\n"
              << "  e.g. " << argv[0] << " localhost 4002\n";
  return 1;
  }

  try {
    networking::Client client{argv[1], argv[2]};
    RunChatClient(client);
  } catch (const std::exception& e) {
      std::cerr << "Error initializing the client: " << e.what() << std::endl;
  }

  return 0;
}

std::vector<Element>& getCurrentHistory(Page pageContext,
                                        std::vector<Element>& mainMenuHistory,
                                        std::vector<Element>& joinGameHistory,
                                        std::vector<Element>& createGameHistory) {
                                          
  if (pageContext == Page::MainMenu) {
    //return mainMenu chat list
    return mainMenuHistory;
  } else if (pageContext == Page::JoinGame) {
    //return joinmenu chat list
    return joinGameHistory;
  } else if (pageContext == Page::CreateGame) {
    //return createMenu chat list
    return createGameHistory;
  } else {
    //default to the main menu history
    return mainMenuHistory;
  }
}

void joinGameRequest(networking::Client& client){
  std::string gameRequest = "join_game";
  client.send(gameRequest);
}

void sendCreateGameRequest(networking::Client& client) {
  std::string gameRequest = "create_game";
  client.send(gameRequest); 
}

Element RenderMainMenu(const std::vector<Element>& history, Component entryField, Component makeBtn, Component joinBtn, Component helpBtn) {
  return vbox({
    window(text("Main Menu"), yframe(vbox(history) | focusPositionRelative(0, 1)) | yflex),
    window(text("Welcome"), entryField->Render() | size(HEIGHT, EQUAL, 3)),
    window(text("Options"), hbox(
        hflow(0.4, makeBtn->Render()), 
        hflow(0.4, joinBtn->Render()), 
        hflow(0.4, helpBtn->Render())
    ) | size(HEIGHT, EQUAL, 3)
    ) | size(HEIGHT, EQUAL, 5),
  }) | color(Color::GreenLight);
}

Element RenderHelpPage(Component backBtn) {
  return yframe(vbox({
    // window(text("Help Page"), vbox(history) | focusPositionRelative(0, 1)),
    window(text("Help Content"), vbox({
      text("Getting Started:"),
      text(" - Select 'Join Game' to see a list of available games."),
      text(" - Select 'Make Game' to start a new game session."),
      text(" - Use F1 to return to the Main Menu."),
      text(" - Use F3 to join a game, F4 to create a new game."),
      separator(),
      text("Gameplay:"),
      text("Currently, we are developing rock paper scissors functionality"),
      text("You will be asked depending on game server configuration to pick which choice, and you will see the winner as such."),
      separator(),
      text("Troubleshooting:"),
      text(" - If you encounter issues, try restarting the client."),
      text(" - Check Readme for further instruction."),
      separator(),
      text("Contact Us:"),
      text(" - Email: klah@socialgaming.com"),
      text(" - Discord: discord.gg/teamklah"),
      separator(),
      text("Credits:"),
      text(" - Developed by Team Klah."),
    }) | size(HEIGHT, GREATER_THAN, 10)),
    
    window(text("Options"), backBtn->Render() | size(HEIGHT, EQUAL, 3))
  })) | yflex | color(Color::BlueLight);
}


Element RenderJoinGamePage(const std::vector<Element>& history, Component joinGameBtn, Component backBtn) {
  return vbox({
          window(text("Join Game"), vbox(history) | focusPositionRelative(0, 1) | yflex),
          window(text("Join Game Content"), joinGameBtn->Render() | size(HEIGHT, EQUAL, 3)),
          window(text("Options"), backBtn->Render() | size(HEIGHT, EQUAL, 3))
  }) | color(Color::RedLight);

}


Element RenderMakeGamePage(const std::vector<Element>& history, Component createGameBtn, Component backBtn) {
  return yframe(vbox({
    window(text("Make Game"), vbox(history) | focusPositionRelative(0, 1) | yflex),
    window(text("Create Game Request"), createGameBtn->Render() | size(HEIGHT, EQUAL, 3)),
    window(text("Options"), backBtn->Render() | size(HEIGHT, EQUAL, 3))
  })) | yflex | color(Color::YellowLight);
  
}


void RunChatClient(networking::Client& client) {
  Page pageContext = Page::MainMenu;
  bool done = false;

  auto onTextEntry = [&done, &client](std::string text) {
    if ("exit" == text || "quit" == text) {
      done = true;
    } else {
      client.send(std::move(text));
    }
  };

  std::string entry;
  std::vector<Element> history;
  std::vector<Element> joinGameHistory;
  std::vector<Element> createGameHistory;
  Component entryField = Input(&entry, "Type Here.");

  std::vector<std::string> menuItems = {"Help", "Join Game", "Make Game"};

  //https://github.com/ArthurSonzogni/FTXUI/discussions/323
  //buttons that won't click through
  auto helpHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component helpBtn = Button("Help Me!! [F2]", [helpHandler] { (*helpHandler)(); });

  //button on main menu to signify joining menu
  auto joinHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component joinBtn = Button("Join Game [F3]", [joinHandler] { (*joinHandler)();});

  //make button showcased on main menu to launch into make game menu
  auto makeHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component makeBtn = Button("Make Game [F4]", [makeHandler] { (*makeHandler)(); });

  //universal back button
  auto backHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component backBtn = Button("Back [Esc]", [backHandler] { (*backHandler)(); });



  /*outer menu buttons*/

  //located in create game menu, strike c to create a game request from the server
  auto createGameHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component createGameBtn = Button("Create Game [F5]", [createGameHandler] { 
      (*createGameHandler)(); 
      // std::string gameRequest = "gamerequest";
      // client.send(gameRequest);
  });

  //located in the join game menu, strike g to join a current game
  auto joinRequestHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component joinGameBtn = Button("Join Game [F6]", [joinRequestHandler] { 
      (*joinRequestHandler)(); 
      // std::string joinRequest = "joinrequest";
      // client.send(joinRequest);
  });

 

  auto renderer = Renderer(entryField, [&history, &createGameHistory, &joinGameHistory, &entryField, &makeBtn, &joinBtn, &helpBtn, &backBtn, &createGameBtn, &joinGameBtn, &pageContext] {
      switch (pageContext) {
        case Page::MainMenu: return RenderMainMenu(history, entryField, makeBtn, joinBtn, helpBtn);
        case Page::HelpMenu: return RenderHelpPage(backBtn);
        case Page::JoinGame: return RenderJoinGamePage(joinGameHistory, joinGameBtn, backBtn);
        case Page::CreateGame: return RenderMakeGamePage(createGameHistory, createGameBtn, backBtn);
        default: return RenderMainMenu(history, entryField, makeBtn, joinBtn, helpBtn);
      }
  });

  auto screen = ScreenInteractive::Fullscreen();

  //handler for specific events caught in the chat windows
  auto handler = CatchEvent(renderer, [&entry, &onTextEntry, &pageContext, &backHandler, &createGameHandler, &joinRequestHandler, &client](const Event& event) {
    if (event == Event::Return) {
      onTextEntry(std::move(entry));
      entry.clear();
      return true;
    } else if (event == Event::F2) {
      // Switch to help page when 'h' is pressed.
      pageContext = Page::HelpMenu;
      return true;
    } 
    else if (event == Event::F3) {
      //  join game page when 'j' is pressed.
      pageContext = Page::JoinGame;
      return true;
    } else if (event == Event::F4) {
      //  to the make game page when 'm' is pressed.
      pageContext = Page::CreateGame;
      return true;
    } 
    else if (event == Event::Escape) {
      // switch to the main page when 'b' is pressed.
      entry.clear();
      pageContext = Page::MainMenu;
      return true;
    }
    else if (event == Event::F5) {
      // send game request through
      // (*createGameHandler)();
      sendCreateGameRequest(client);
      return true;
    }
    else if (event == Event::F6) {
      // send game request through
      joinGameRequest(client);
      return true;
    }
    
    return false;
  });

  const int UPDATE_INTERVAL_IN_MS = 50;
  Loop loop(&screen, handler);
  while (!done && !client.isDisconnected() && !loop.HasQuitted()) {
    try {
      client.update();
    } catch (std::exception& e) {
      auto& currentHistory = getCurrentHistory(pageContext, history, joinGameHistory, createGameHistory);

      currentHistory.push_back(text("Exception from Client update:"));
      currentHistory.push_back(text(e.what()));
      done = true;
    }

    auto response = client.receive();
    if (!response.empty()) {
      auto& currentHistory = getCurrentHistory(pageContext, history, joinGameHistory, createGameHistory);

      currentHistory.push_back(paragraphAlignLeft(response));
      screen.RequestAnimationFrame();
      
    }

    loop.RunOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_IN_MS));
  }
}
