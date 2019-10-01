add_library(ffGraph_JSON
    ${CMAKE_SOURCE_DIR}/src/JSON/deserializer.cpp
)

target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/src/util)