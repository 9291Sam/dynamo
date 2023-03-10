#version 460

layout(location = 0) in vec3 in_pos_world;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(push_constant) uniform PushConstants
{
    mat4 view_projection;
    mat4 model;
} in_push_constants;

layout(binding = 0) uniform UniformBuffer
{
    vec3 light_position;
    vec4 light_color;
} in_uniform_buffer;

layout(binding = 1) uniform sampler2D in_texture_sampler;

layout(location = 0) out vec4 out_color;

const vec4 ambient_light = vec4(1.0, 1.0, 1.0, 0.01);

void main() 
{
    out_color = vec4(in_color, 1.0);
}