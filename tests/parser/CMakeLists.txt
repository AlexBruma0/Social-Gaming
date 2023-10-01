

# Use the library in a demo program.
add_executable(parser_test)
target_sources(parser_test
  PRIVATE
    parser_test.cpp
)
set_target_properties(parser_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/parser)
target_link_libraries(parser_test
  tree-sitter-socialgaming
  cpp-tree-sitter
  ${GTEST_BOTH_LIBRARIES}
  parser
)

add_test(NAME parser_test COMMAND parser_test)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET parser_test PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/games/ ${CMAKE_BINARY_DIR}/resources/games/)
