add_executable(gameStateTest)

target_sources(gameStateTest
  PRIVATE
    gameStateTest.cpp
)

set_target_properties(gameStateTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/gameState)

target_link_libraries(gameStateTest
    tree-sitter-socialgaming
    cpp-tree-sitter
    nlohmann_json
    gameState ${GTEST_BOTH_LIBRARIES} pthread
    parser
    utils)

add_test(NAME gameStateTest COMMAND gameStateTest)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET gameStateTest PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/games/ ${CMAKE_BINARY_DIR}/resources/games/)

