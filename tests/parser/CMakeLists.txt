# Build and configure parser_test
add_executable(parser_test parser_test.cpp)
add_executable(rule_test rule_test.cpp)
set_property(TARGET rule_test  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/parser)
set_property(TARGET parser_test  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/parser)
add_test(NAME parser_test COMMAND parser_test)
add_test(NAME rule_test COMMAND rule_test)

# parser link libraries
target_link_libraries(parser_test
        tree-sitter-socialgaming
        cpp-tree-sitter
        nlohmann_json
        ${GTEST_BOTH_LIBRARIES}
        pthread
        parser
        utils
)

# rule link libraries
target_link_libraries(rule_test
        tree-sitter-socialgaming
        cpp-tree-sitter
        ${GTEST_BOTH_LIBRARIES}
        pthread
        rules
        parser
)

#Definitions for parser test to find resources
target_compile_definitions(parser_test
    PRIVATE
    -DRPS_LOCATION="${CMAKE_SOURCE_DIR}/resources/games/rock-paper-scissors.game"
    -DEMPTY_GAME_LOCATION="${CMAKE_SOURCE_DIR}/resources/games/empty.game"
)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET parser_test PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/games/
        ${CMAKE_BINARY_DIR}/resources/games/
)

#Automatically runs the tests
#Need to add one per test
add_custom_command(TARGET rule_test POST_BUILD COMMAND ctest -R "rule_test" --output-on-failure)
add_custom_command(TARGET parser_test POST_BUILD COMMAND ctest -R "parser_test" --output-on-failure)
