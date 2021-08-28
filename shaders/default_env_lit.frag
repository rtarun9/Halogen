#version 450

layout (location = 0) in vec3 in_color;
layout (location = 0) out vec4 out_frag_color;

layout (set = 0, binding = 1) uniform EnvironmentData
{
    vec4 m_fog_color;
    vec4 m_fog_distance;
    vec4 m_ambient_color;
} environment_data;

void main()
{
    out_frag_color = vec4(in_color + environment_data.m_ambient_color.xyz, 1.0f);
}