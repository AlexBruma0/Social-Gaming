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

  auto onTextEntry = [&done, &client] (std::string text) {
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


  auto helpHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component helpBtn = Button("Help Me!!", [helpHandler] { (*helpHandler)(); });

  auto joinHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component joinBtn = Button("Join Game", [joinHandler] { (*joinHandler)(); });

  auto makeHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component makeBtn = Button("Make Game", [makeHandler] { (*makeHandler)(); });

  // Define the core appearance with a renderer for the components.
  // A `Renderer` takes input consuming components like `Input` and
  // produces a DOM to visually represent their current state.
  auto renderer = Renderer(entryField, [&history,&entryField,&makeBtn, &joinBtn, &helpBtn] {
    return vbox({
      window(text("Main Menu"),
        yframe(
          vbox(history) | focusPositionRelative(0, 1)
        )
      ) | yflex,

      window(text("Welcome"),
        entryField->Render()
      ) | size(HEIGHT, EQUAL, 3),

      window(text("Options"),
        hbox(
          hflow(0.4, makeBtn->Render()), // Add spacing between buttons.
          hflow(0.4, joinBtn->Render()), // Add spacing between buttons.
          hflow(0.4, helpBtn->Render())
          
        ) | size(HEIGHT, EQUAL, 3)
      ) | size(HEIGHT, EQUAL, 5),

    }) | color(Color::GreenLight);
  });

  auto screen = ScreenInteractive::Fullscreen();

  // Bind a handler for "return" presses that consumes the text entered
  // so far.
  auto handler = CatchEvent(renderer, [&entry,&onTextEntry](const Event& event) {
    if (event == Event::Return) {
      onTextEntry(std::move(entry));
      entry.clear();
      return true;
    }
    return false;
  });

//MAIN LOOP
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
