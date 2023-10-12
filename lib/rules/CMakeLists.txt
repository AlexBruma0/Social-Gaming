
add_library(rules
        src/ruleNode.cpp
        src/ruleNodeSupport.cpp
)

target_include_directories(rules
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


set_target_properties(rules
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)

install(TARGETS rules
  ARCHIVE DESTINATION lib
)