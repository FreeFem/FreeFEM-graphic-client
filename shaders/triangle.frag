#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 colorIn;
layout(location = 0) out vec4 outFragmentColor;

layout(push_constant) uniform PushConstants
{
	float animationTime;
} pushConstants;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	float hue = fract(pushConstants.animationTime / 7000.0);
	outFragmentColor = vec4(hsv2rgb(vec3(hue, 1.0, 1.0)), 1.0);
}