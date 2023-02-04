#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(push_constant) uniform PushConstantsGLSL
{
    mat4 model_view_projection_matrix;
} in_push_constants;

layout(location = 0) out vec3 out_frag_color;

void main() 
{
    gl_Position = in_push_constants.model_view_projection_matrix * vec4(in_position, 1.0);
    out_frag_color = in_color;
}