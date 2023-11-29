
add_library(gameState src/GameState.cpp src/GameVariables.cpp)

target_include_directories(gameState
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_link_libraries(
        gameState
        nlohmann_json
        rules
)

install(TARGETS gameState ARCHIVE DESTINATION lib)