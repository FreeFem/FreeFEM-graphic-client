add_library(ffGraph_NET
    ffClient.cpp
)

target_include_directories(ffGraph_NET PRIVATE ${CMAKE_SOURCE_DIR}/extern/asio/asio/include)
target_include_directories(ffGraph_NET PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/)
target_link_libraries(ffGraph_NET Threads::Threads)