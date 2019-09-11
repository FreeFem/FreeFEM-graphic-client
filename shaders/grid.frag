#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 ColorIn;
layout(origin_upper_left) in vec4 gl_FragCoord;

float grid(vec2 st, float res)
{
    vec2 grid = fract(st * res);
    return (step(res, grid.x) * step(res, grid.y));
}

void main()
{
    vec2 grid_uv = gl_FragCoord.xy * 1;
    float x = grid(grid_uv, 0.1557893f);
    gl_FragColor.rgb = vec3(1.f) * x;
    gl_FragColor.a = 1.0f;
}