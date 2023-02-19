#version 460

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec3 in_pos_world;
layout(location = 2) in vec3 in_normal;

layout(push_constant) uniform PushConstantsGLSL
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

const vec4 ambient_light = vec4(1.0, 1.0, 1.0, 0.02);

void main() 
{
    const vec3 normal_vector = normalize(in_normal);
    const vec3 distance_to_light = in_uniform_buffer.light_position - in_pos_world;
    const vec3 direction_to_light = normalize(distance_to_light);
    
    out_color.xyz =
        (in_color * in_uniform_buffer.light_color.xyz * max(dot(normal_vector, direction_to_light), 0.0) *
        in_uniform_buffer.light_color.w *
        (1.0 / sqrt((dot(distance_to_light, distance_to_light)))));
}