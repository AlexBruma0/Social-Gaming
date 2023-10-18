
add_library(parser
        src/gameParser.cpp
)

target_include_directories(parser
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


set_target_properties(parser
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)
target_link_libraries( parser
tree-sitter-socialgaming
cpp-tree-sitter)
install(TARGETS parser
  ARCHIVE DESTINATION lib
)