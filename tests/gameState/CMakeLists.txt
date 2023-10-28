add_executable(gameStateTest gameStateTest.cpp)
add_test( NAME gameStateTest COMMAND gameStateTest)

set_target_properties(gameStateTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/gameState)

target_link_libraries(gameStateTest
        tree-sitter-socialgaming
        cpp-tree-sitter
        nlohmann_json
        gameState ${GTEST_BOTH_LIBRARIES} pthread
        parser
        utils
)

target_compile_definitions(gameStateTest
    PRIVATE
    -DRPS_LOCATION="${CMAKE_SOURCE_DIR}/resources/games/rock-paper-scissors.game"
    -DEMPTY_GAME_LOCATION="${CMAKE_SOURCE_DIR}/resources/games/empty.game"
)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET gameStateTest PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/games/ ${CMAKE_BINARY_DIR}/resources/games/
)

# Command to automatically run the test
add_custom_command(TARGET gameStateTest POST_BUILD COMMAND ctest -R "gameStateTest" --output-on-failure)
