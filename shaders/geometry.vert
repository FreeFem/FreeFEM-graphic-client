#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorIn;
layout(location = 0) out vec3 colorOut;

layout(push_constant) uniform PushConstant {
	mat4 MVP;
} PushConst;

void main()
{
	colorOut = colorIn;
	gl_Position = PushConst.MVP * vec4(position, 1.0);
}