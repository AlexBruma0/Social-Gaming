#include <memory>
#include <string>
#include <vector>
#include <cmath> 
#include <chrono>

#include "Client.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/animation.hpp"

using namespace ftxui;

//callback struct to create actions for the buttons
struct ButtonHandler {
    bool& done;
    networking::Client& client;
    std::string& entry;

    void operator()() {}
};


void RunChatClient(networking::Client& client);

void HandleButtonClick(int& page, networking::Client& client, const std::string& buttonLabel);


Element drawFrame(std::string message, float angle, std::string footer) {
  const int radius = 10; 
  int x = static_cast<int>(radius * cos(angle));
  int y = static_cast<int>(radius * sin(angle));

  // sets up the location of animated pixels
  std::string circle(radius * 2 + 1, ' ');
  circle[radius + x] = '*'; 

  //return box
  return vbox({
    text(message) | center,
    text(circle) | center,
    text(footer) | center,
  }) | border;
}

//animates box
void RunAnimation(ScreenInteractive& screen, float& angle, std::atomic<bool>& running) {
  // https://arthursonzogni.github.io/FTXUI/classftxui_1_1ScreenInteractive.html
  while (running) {
    //slows down animation
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // adjusts angle 
    angle += 0.1f; 
    //keeps angle in 2pi distance, to stop from floating off of screen
    if (angle > 2 * M_PI) {
      angle -= 2 * M_PI;
    }
    //renders every single movement cause by angle adjustment
    screen.PostEvent(Event::Custom); 
  }
}

void startAnimation() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::string message = "WELCOME TO KLAH SOCIAL GAMING";
  std::string footer = "Loading...";
  // angle for animated pixels
  float angle = 0.0f; 
   // atomic boolean to stop thread execution
   //https://stackoverflow.com/questions/62830569/boolean-stop-signal-between-threads
  std::atomic<bool> running(true);

  // draws back and forth animation
  auto component = Renderer([&] {
    return drawFrame(message, angle, footer);
  });

  // setup container component of animation
  auto main_container = Container::Vertical({
    component,
  });

  auto exit_loop = screen.ExitLoopClosure();

  // thread running the animation
  //Animator class in ftxui did not correspond when building
  std::thread animation_thread([&] {
    RunAnimation(screen, angle, running);
  });

  // timer thread running in parallel with animation
  std::thread timer_thread([&] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    // stop  animation
    running = false; 
    exit_loop(); 
  });

  //loop on screen display/animation
  screen.Loop(main_container);

  // join up threads
  if (animation_thread.joinable()) {
    animation_thread.join(); 
  }
  if (timer_thread.joinable()) {
    timer_thread.join();
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
      std::cerr << "Usage: \n  " << argv[0] << " <ip address> <port>\n"
              << "  e.g. " << argv[0] << " localhost 4002\n";
  return 1;
  }

  startAnimation();

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


  std::string entry;
  std::vector<Element> history;
  Component entryField = Input(&entry, "Type Here.");

  std::vector<std::string> menuItems = {"Help", "Join Game", "Make Game"};

  //https://github.com/ArthurSonzogni/FTXUI/discussions/323
  //buttons that won't click through
  auto helpHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component helpBtn = Button("Help Me!! [h]", [helpHandler] { (*helpHandler)(); });

  //button on main menu to signify joining menu
  auto joinHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component joinBtn = Button("Join Game [j]", [joinHandler] { (*joinHandler)();});

  //make button showcased on main menu to launch into make game menu
  auto makeHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component makeBtn = Button("Make Game [m]", [makeHandler] { (*makeHandler)(); });

  //universal back button
  auto backHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component backBtn = Button("Back [b]", [backHandler] { (*backHandler)(); });


  /*outer menu buttons*/

  //located in create game menu, strike c to create a game request from the server
  auto createGameHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component createGameBtn = Button("Create Game [c]", [createGameHandler] { 
      (*createGameHandler)(); 
      // std::string gameRequest = "gamerequest";
      // client.send(gameRequest);
  });

  //located in the join game menu, strike g to join a current game
  auto joinRequestHandler = std::make_shared<ButtonHandler>(ButtonHandler{done, client, entry});
  Component joinGameBtn = Button("Join Game [r]", [joinRequestHandler] { 
      (*joinRequestHandler)(); 
      // std::string joinRequest = "joinrequest";
      // client.send(joinRequest);
  });

 
  auto renderer = Renderer(entryField, [&history, &entryField, &makeBtn, &joinBtn, &helpBtn, &backBtn, &createGameBtn, &joinGameBtn, &page] {
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
          window(text("Join Game Content"), joinGameBtn->Render() | size(HEIGHT, EQUAL, 3)),
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
  });

  auto screen = ScreenInteractive::Fullscreen();

  //handler for specific events caught in the chat windows
  auto handler = CatchEvent(renderer, [&entry, &onTextEntry, &page, &backHandler, &createGameHandler, &joinRequestHandler](const Event& event) {
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
      (*createGameHandler)();
      page = 0;
      return true;
    }
    else if (event == Event::Character('g')) {
      // send game request through
      (*joinRequestHandler)();
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
