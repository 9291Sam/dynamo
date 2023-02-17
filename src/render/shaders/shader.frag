#version 450

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;

# error test uniform buffers

layout(binding = 0) uniform UniformBuffer
{
    GL_R16 number;
    vec4 l;
} in_uniform_buffer;


layout(location = 0) out vec4 out_color;

void main() 
{
    out_color = vec4(in_color, 1.0);
}