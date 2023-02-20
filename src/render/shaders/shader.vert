#version 460

layout(location = 0) in vec3 in_position;
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

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec3 out_pos_world;
layout(location = 2) out vec3 out_normal;

void main() 
{
    const vec4 pos_world_affine = in_push_constants.model * vec4(in_position, 1.0);

    gl_Position = in_push_constants.view_projection * pos_world_affine;
    out_color = in_color;
    out_pos_world = pos_world_affine.xyz * pos_world_affine.w;
    out_normal = inverse(transpose(mat3(in_push_constants.model))) * in_normal;
}