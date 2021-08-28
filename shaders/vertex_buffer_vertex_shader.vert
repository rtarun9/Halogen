#version 450

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

layout (location = 0) out vec3 out_frag_color;

layout (push_constant) uniform constants
{
    mat4 m_model_matrix;
}PushConstants;

layout (set = 0, binding = 0) uniform camera_buffer
{
    mat4 m_view_mat;
    mat4 m_projection_mat;
    mat4 m_vp_mat;
}CameraBuffer;

void main()
{
    gl_Position = CameraBuffer.m_vp_mat * PushConstants.m_model_matrix * vec4(in_position.xyz, 1.0f);
    out_frag_color = in_color;
}
