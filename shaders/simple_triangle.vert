#version 450

#extension GL_KHR_vulkan_glsl : enable

void main()
{
	const vec3 POSITIONS[3] = vec3[3]
	(
		vec3(-1.0f, 1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f)
	);

	gl_Position = vec4(POSITIONS[gl_VertexIndex], 1.0f);
}