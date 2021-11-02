#version 450

layout (location = 0) in vec3 in_color;

// declare output will be on location 0.
layout (location = 0) out vec4 o_frag_color;

void main()
{
	o_frag_color = vec4(in_color, 1.0f);
}