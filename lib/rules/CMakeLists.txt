
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

target_link_libraries(rules
        nlohmann_json
        tree-sitter-socialgaming
        gameState
        cpp-tree-sitter
        utils
        networking
)

install(TARGETS rules
  ARCHIVE DESTINATION lib
)