#version 450

// declare output will be on location 0.
layout (location = 0) out vec4 o_frag_color;

void main()
{
	o_frag_color = vec4(0.3f, 0.5f, 0.7f, 1.0f);
}