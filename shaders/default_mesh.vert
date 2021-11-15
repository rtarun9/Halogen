#version 450

#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

layout (location = 0) out vec3 frag_color;

layout (push_constant) uniform constants
{
	mat4 m_transform_mat;
} PushConstants;

layout (set = 0, binding = 0) uniform CameraBuffer
{
	mat4 m_view_mat;
	mat4 m_projection_mat;

	mat4 m_projection_view_mat;
} cameraBuffer;

void main()
{
	gl_Position = cameraBuffer.m_projection_view_mat * PushConstants.m_transform_mat * vec4(in_position, 1.0f);

	frag_color = in_color;
}