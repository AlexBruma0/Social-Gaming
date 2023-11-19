enable_testing()

add_executable(messageQueue_test messageQueue_test.cpp)
set_property(TARGET messageQueue_test  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/parser)
add_test(NAME messageQueue_test COMMAND parser_test)

add_executable(networking_test networking_test.cpp)
set_target_properties(networking_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/networking)
target_link_libraries(networking_test networking ${GTEST_BOTH_LIBRARIES} pthread)
add_test(NAME networking_test COMMAND networking_test)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(
        TARGET networking_test PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/networking/
        ${CMAKE_BINARY_DIR}/resources/networking/
)

#Automatically runs the tests
#Commented out temporarily because the tests don't work
#add_custom_command(TARGET networking_test POST_BUILD COMMAND ctest "networking_test" --output-on-failure)