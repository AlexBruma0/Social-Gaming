
add_library(parser
        src/gameParser.cpp
)
set(CMAKE_CXX_STANDARD 20)
target_include_directories(parser
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

target_compile_features(parser PUBLIC cxx_std_20)
set_target_properties(parser
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)
target_link_libraries( parser
tree-sitter-socialgaming
cpp-tree-sitter)
install(TARGETS networking
  ARCHIVE DESTINATION lib
)