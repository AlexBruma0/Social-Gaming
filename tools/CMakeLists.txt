
if (NETWORKING_EMSCRIPTEN_BUILD)
  message("Skipping server and NCurses client under emscripten build.")
else()
  add_subdirectory(server)

  # Change this to another client if you want to switch  
  add_subdirectory(client-ftxui)
endif()