add_library(networking src/Server.cpp src/Client.cpp src/MessageQueue.cpp src/gameServer.cpp)

target_include_directories(networking
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(networking
        PUBLIC
        nlohmann_json
        parser
        rules
)

if (NETWORKING_EMSCRIPTEN_BUILD)
  target_link_libraries(networking
          PRIVATE
          websocket.js
  )
else()
  find_package(Boost 1.72 COMPONENTS system REQUIRED)
  target_link_libraries(networking
          PRIVATE
          ${Boost_LIBRARIES}
  )
  target_include_directories(networking
          PRIVATE
          ${Boost_INCLUDE_DIR}
  )
endif()

install(TARGETS networking ARCHIVE DESTINATION lib)