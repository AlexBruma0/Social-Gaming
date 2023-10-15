add_executable(game_test game_test.cpp)
set_target_properties(game_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/game)
target_link_libraries(game_test lib_game ${GTEST_BOTH_LIBRARIES} pthread)
add_test(NAME game_test COMMAND game_test)
