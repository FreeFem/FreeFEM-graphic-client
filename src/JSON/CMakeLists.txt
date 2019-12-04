add_library(ffGraph_JSON
    ${CMAKE_SOURCE_DIR}/src/JSON/ThreadQueue.cpp
    ${CMAKE_SOURCE_DIR}/src/JSON/Import.cpp
    ${CMAKE_SOURCE_DIR}/src/JSON/ImportIso.cpp
    ${CMAKE_SOURCE_DIR}/src/JSON/IO.cpp
    ${CMAKE_SOURCE_DIR}/src/JSON/LabelTable.cpp
)

target_include_directories(ffGraph_JSON PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/extern/glm)
target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/extern/CTPL)
target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_include_directories(ffGraph_JSON PRIVATE ${CMAKE_SOURCE_DIR}/src)