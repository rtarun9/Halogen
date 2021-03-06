#version 450

#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) out vec3 out_color;

void main()
{
	const vec3 POSITIONS[3] = vec3[3]
	(
		vec3(-0.5f, 0.5f, 0.0f),
		vec3(0.0f, -0.5f, 0.0f),
		vec3(0.5f, 0.5f, 0.0f)
	);

	const vec3 COLORS[3] = vec3[3]
	(
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f)
	);

	out_color = COLORS[gl_VertexIndex];
	gl_Position = vec4(POSITIONS[gl_VertexIndex], 1.0f);
}