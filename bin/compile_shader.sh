echo "Compiling RGB triangle shaders"

glslc ../shaders/rgb_triangle_vertex_shader.vert -o rgb_triangle_vertex_shader.spv
glslc ../shaders/rgb_triangle_fragment_shader.frag -o rgb_triangle_fragment_shader.spv

echo "Compiling PLAIN triangle shaders"

glslc ../shaders/plain_triangle_vertex_shader.vert -o plain_triangle_vertex_shader.spv
glslc ../shaders/plain_triangle_fragment_shader.frag -o plain_triangle_fragment_shader.spv

echo "Compiled shaders!"
