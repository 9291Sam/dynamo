#version 450

layout(location = 0) in vec3 in_frag_color;
layout(location = 1) in vec3 in_frag_pos;
layout(location = 2) in vec3 in_normal_world;

layout(location = 0) out vec4 out_color;

const vec3 light_position = vec3(-25.0, 25.0, 25.0);
const vec4 light_color    = vec4(1.0, 1.0, 1.0, 300.0);
const vec4 ambient_color  = vec4(1.0, 1.0, 1.0, 0.02);

void main() 
{
  const vec3 directionToLight = light_position - in_frag_pos;
  const float attenuationFactor = 1.0 / dot(directionToLight, directionToLight);

  const vec3 lightColor = light_color.xyz * light_color.w * attenuationFactor;
  const vec3 ambientLight = ambient_color.xyz * ambient_color.w;
  const vec3 diffuseLight = lightColor * max(dot(normalize(in_normal_world), normalize(directionToLight)), 0.0);

  out_color = vec4((diffuseLight + ambientLight) * in_frag_color, 1.0);
}