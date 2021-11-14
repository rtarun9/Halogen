#version 450

layout (location = 0) in vec3 in_color;
layout (location = 0) out vec4 frag_color;

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() 
{
	frag_color = vec4(rand(in_color.xy) * in_color.xyz, 1.0f);
}