#version 460

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

struct ObjectData
{
	mat4 m_model_mat;
};

layout (std140, set = 1, binding = 0) readonly buffer  ObjectBuffer
{
	ObjectData objects[];	
} objectBuffer;

void main()
{
	mat4 model_mat = objectBuffer.objects[gl_BaseInstance].m_model_mat;
	mat4 transform_mat = cameraBuffer.m_projection_view_mat * model_mat;

	gl_Position = transform_mat * vec4(in_position, 1.0f);

	frag_color = in_color;
}