#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Client.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"


struct ButtonHandler {
    bool& done;
    networking::Client& client;
    std::string& entry;

    void operator()() {}
};


void RunChatClient(networking::Client& client);

void HandleButtonClick(int& page, networking::Client& client, const std::string& buttonLabel);

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


void RunChatClient(networking::Client& client) {
  int page = 0;
  bool done = false;

  auto onTextEntry = [&done, &client](std::string text) {
    if ("exit" == text || "quit" == text) {
      done = true;
    } else {
      client.send(std::move(text));
    }
  };

  using namespace ftxui;

  std::string entry;
  std::vector<Element> history;
  Component entryField = Input(&entry, "Type Here.");

  std::vector<std::string> menuItems = {"Help", "Join Game", "Make Game"};

  //https://github.com/ArthurSonzogni/FTXUI/discussions/323
  //buttons that won't click through
  auto helpHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component helpBtn = Button("Help Me!! [h]", [helpHandler] { (*helpHandler)(); });

  auto joinHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component joinBtn = Button("Join Game [j]", [joinHandler] { (*joinHandler)();});

  auto makeHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component makeBtn = Button("Make Game [m]", [makeHandler] { (*makeHandler)(); });

  auto backHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component backBtn = Button("Back [b]", [backHandler] { (*backHandler)(); });

  auto createGameHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component createGameBtn = Button("Create Game [c]", [createGameHandler] { 
      (*createGameHandler)(); 
      std::string gameRequest = "gamerequest";
      client.send(gameRequest);
  });

 
  auto renderer = Renderer(entryField, [&history, &entryField, &makeBtn, &joinBtn, &helpBtn, &backBtn, &createGameBtn, &page] {
    //if page == 0 this is the main menu
    if (page == 0) {
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
      //page 1 is the help page
    } else if (page == 1) {
      return vbox({
          window(text("Help Page"), vbox(history) | focusPositionRelative(0, 1) | yflex),
          window(text("Help Content"), text("This is the help content. Press 'Back' to return.") | size(HEIGHT, EQUAL, 3)),
          window(text("Options"), backBtn->Render() | size(HEIGHT, EQUAL, 3))
      }) | color(Color::BlueLight);
      //page 2 is the join game page
    } else if (page == 2) {
      return vbox({
          window(text("Join Game"), vbox(history) | focusPositionRelative(0, 1) | yflex),
          window(text("Join Game Content"), text("This is the Join Game content. Press 'Back' to return.") | size(HEIGHT, EQUAL, 3)),
          window(text("Options"), backBtn->Render() | size(HEIGHT, EQUAL, 3))
      }) | color(Color::RedLight);
      //page 3 is the make game page
    } else if (page == 3) {
      return vbox({
          window(text("Make Game"), vbox(history) | focusPositionRelative(0, 1) | yflex),
          window(text("Create Game Request"), createGameBtn->Render() | size(HEIGHT, EQUAL, 3)),
          window(text("Options"), backBtn->Render() | size(HEIGHT, EQUAL, 3))
      }) | color(Color::YellowLight);
    }
  });

  auto screen = ScreenInteractive::Fullscreen();

  //handler for specific events caught in the chat windows
  auto handler = CatchEvent(renderer, [&entry, &onTextEntry, &page, &backHandler, &createGameHandler](const Event& event) {
    if (event == Event::Return) {
      onTextEntry(std::move(entry));
      entry.clear();
      return true;
    } else if (event == Event::Character('h')) {
      // Switch to help page when 'h' is pressed.
      page = 1;
      return true;
    } else if (event == Event::Character('j')) {
      //  join game page when 'j' is pressed.
      page = 2;
      return true;
    } else if (event == Event::Character('m')) {
      //  to the make game page when 'm' is pressed.
      page = 3;
      return true;
    } 
    else if (event == Event::Character('b')) {
      // switch to the main page when 'b' is pressed.
      page = 0;
      return true;
    }
    else if (event == Event::Character('c')) {
      // send game request through
      createGameHandler();
      page = 0;
      return true;
    }
    // else if (event == Event::Mouse) {
    //     // Mouse mouse = event.mouse();
    //     // int x = mouse.x;
    //     // int y = mouse.y;
    // }
    return false;
  });

  const int UPDATE_INTERVAL_IN_MS = 50;
  Loop loop(&screen, handler);
  while (!done && !client.isDisconnected() && !loop.HasQuitted()) {
    try {
      client.update();
    } catch (std::exception& e) {
      history.push_back(text("Exception from Client update:"));
      history.push_back(text(e.what()));
      done = true;
    }

    auto response = client.receive();
    if (!response.empty()) {
      history.push_back(paragraphAlignLeft(response));
      screen.RequestAnimationFrame();
    }

    loop.RunOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_IN_MS));
  }
}
