
if (NETWORKING_EMSCRIPTEN_BUILD)
  message("Skipping server and NCurses client under emscripten build.")
else()
  add_subdirectory(server)
  if (ENABLE_NCURSES_CLIENT)
    add_subdirectory(client-ncurses)
  endif()
endif()