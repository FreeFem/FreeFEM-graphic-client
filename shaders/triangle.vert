#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorIn;
layout(location = 0) out vec3 colorOut;

layout(binding = 0) uniform GlobalUniformInfos
{
    mat4 model;
    mat4 view;
    mat4 proj;
} GlobalUniformInfos;

void main()
{
	colorOut = colorIn;
	gl_Position = GlobalUniformInfos.proj * GlobalUniformInfos.view * GlobalUniformInfos.model * vec4(position.xy, 0.0, 1.0);
}