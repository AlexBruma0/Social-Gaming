
add_library(lib_game 
        src/Game.cpp
        src/Config.cpp
)

target_include_directories(lib_game PUBLIC "../constants/")
target_include_directories(lib_game
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


set_target_properties(lib_game
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
)

target_link_libraries(lib_game PUBLIC model)