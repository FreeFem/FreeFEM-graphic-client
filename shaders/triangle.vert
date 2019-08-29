#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorIn;
layout(location = 0) out vec3 colorOut;

layout (binding = 0) uniform GlobalState {
    mat4 projection;
	mat4 view;
	mat4 model;
} GlobalUniformInfos;

mat4 viewMatrix(vec3 eye, vec3 center, vec3 up) {
	vec3 f = normalize(center - eye);
	vec3 s = normalize(cross(f, up));
	vec3 u = cross(s, f);
	return mat4(
		vec4(s, -dot(s, eye)),
		vec4(u, -dot(u, eye)),
		vec4(-f, -dot(f, eye)),
		vec4(0.0, 0.0, 0.0, 1)
	);
}

void main()
{
	colorOut = colorIn;
	gl_Position = GlobalUniformInfos.view * vec4(position, 1.0);
}