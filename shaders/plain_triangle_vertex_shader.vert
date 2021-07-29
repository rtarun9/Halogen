#version 450

//positions of vertices (top left is -1, -1).
const vec2 positions[3] = vec2[]
(
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex].xy, 0.0f, 1.0f);
}
