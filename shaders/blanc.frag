#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec4 outFragmentColor;

void main()
{
    outFragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}