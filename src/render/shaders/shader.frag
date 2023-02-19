#version 460

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;

layout(binding = 0) uniform UniformBuffer
{
    int number_of_lights;
    vec4[32] lights;
} in_uniform_buffer;

layout(location = 0) out vec4 out_color;

const float ambient_light_power = 0.02;

void main() 
{
    out_color = vec4(in_color.xyz * ambient_light_power, 1.0);
    
    for (int i = 0; i < in_uniform_buffer.number_of_lights; ++i)
    {
        const vec3 normal_vector = normalize(in_normal);
        const vec3 distance_to_light = in_uniform_buffer.lights[i].xyz - in_pos;
        const vec3 direction_to_light = normalize(distance_to_light);

        // Diffuse lighting component
        out_color.xyz += 
            (in_color * max(dot(normal_vector, direction_to_light), 0.0) * in_uniform_buffer.lights[i].w *
            (1.0 / sqrt((dot(distance_to_light, distance_to_light)))));
    }
}