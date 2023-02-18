#version 460

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(push_constant) uniform PushConstantsGLSL
{
    mat4 model_view_projection_matrix;
} in_push_constants;

layout(binding = 0) uniform UniformBuffer
{
    int number_of_lights;
    vec4[32] lights;
} in_uniform_buffer;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec3 out_pos;
layout(location = 2) out vec3 out_normal;

void main() 
{
    gl_Position = in_push_constants.model_view_projection_matrix * vec4(in_position, 1.0);
    
    out_color = in_color;
    out_pos = in_position;
    out_normal = in_normal;
}