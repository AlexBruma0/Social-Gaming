add_library(parser
        src/gameParser.cpp
)

add_library(utils
        src/utils.cpp
)

target_include_directories(parser
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(utils
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(utils
        nlohmann_json
        tree-sitter-socialgaming
        cpp-tree-sitter
)

target_link_libraries(parser
        nlohmann_json
        tree-sitter-socialgaming
        cpp-tree-sitter
        rules
        utils
)

install(TARGETS parser
        ARCHIVE DESTINATION lib
)
