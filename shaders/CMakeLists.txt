# Compiling shaders
if (UNIX)
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator")
else (UNIX)
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator.exe")
endif(UNIX)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "*.frag"
    "*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spirv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(FreeFEM_graphic_client Shaders)

add_custom_command(TARGET FreeFEM_graphic_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "${PROJECT_BINARY_DIR}/shaders"
        )
