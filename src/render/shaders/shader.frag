#version 460

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;

layout(binding = 0) uniform UniformBuffer
{
    uint number;
    vec4 l;
} in_uniform_buffer;


layout(location = 0) out vec4 out_color;

void main() 
{
    out_color = vec4(in_uniform_buffer.l.x, in_color.yz, 1.0);
}