
add_executable(chatserver
  chatserver.cpp
)

set_target_properties(chatserver
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
                      PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(chatserver
  networking
  ${CMAKE_THREAD_LIBS_INIT}
)

# Command for adding something into the resource folder
# Need to change the target
add_custom_command(TARGET chatserver PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/networking/ ${CMAKE_BINARY_DIR}/resources/networking/)

install(TARGETS chatserver
  RUNTIME DESTINATION bin
)
