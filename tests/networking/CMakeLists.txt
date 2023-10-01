add_executable(networking_test networking_test.cpp)
set_target_properties(networking_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/networking)
target_link_libraries(networking_test networking ${GTEST_BOTH_LIBRARIES})
add_test(NAME networking_test COMMAND networking_test)


# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET networking_test PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/networking/ ${CMAKE_BINARY_DIR}/resources/networking/)