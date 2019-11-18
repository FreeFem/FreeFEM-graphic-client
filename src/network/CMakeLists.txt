add_library(ffGraph_NET
    ffClient.cpp
)

if (WIN32)
    target_link_libraries(ffGraph_NET wsock32 ws2_32)
endif (WIN32)
target_include_directories(ffGraph_NET PRIVATE ${CMAKE_SOURCE_DIR}/extern/asio/asio/include)
target_include_directories(ffGraph_NET PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_link_libraries(ffGraph_NET Threads::Threads)