

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
  ${GTEST_BOTH_LIBRARIES} pthread
  parser
)

add_test(NAME parser_test COMMAND parser_test)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET parser_test PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/games/ ${CMAKE_BINARY_DIR}/resources/games/)


#add_executable(rule_test)
#target_sources(rule_test
#  PRIVATE
#  rule_test.cpp
#)
#set_target_properties(rule_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/parser)
#target_link_libraries(rule_test
#  tree-sitter-socialgaming
#  cpp-tree-sitter
#  ${GTEST_BOTH_LIBRARIES} pthread
#  rules
#  parser
#)
#
#add_test(NAME rule_test COMMAND rule_test)
