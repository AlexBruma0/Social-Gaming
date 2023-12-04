
add_executable(gameserver
        server_executable.cpp
)

set_target_properties(gameserver
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
                      PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(gameserver
                      networking
                      parser
                      gameState
                      ${CMAKE_THREAD_LIBS_INIT}
)


install(TARGETS gameserver
        RUNTIME DESTINATION bin
)
