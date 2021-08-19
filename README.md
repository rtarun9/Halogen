# Halogen
A simple and small graphics engine. This project uses SDL2 for windowing / input, and Vulkan for the graphics backend. 
The purpose of making this engine is to : try out graphics programming concepts, learn more about GPU's, and improve my programming skills.
Since this is not meant to be a full on "Graphics engine" and more of a test project for learning, the code will be very messy.
# Building the project
The bin directory contains two scripts. Use the build.sh script to build the project. 
You will need to have a C++ 17 Compiler, Cmake, Glslc (Google's GLSL to Spir-V compiler), and SDL2 installed. 
If you do not want to use find_package() in Cmake, you need to modify CMakeLists and add 
your own paths.

# External libraries
Uses vk_bootstrap for a clean way to initialize core parts of vulkan. \
Link to vk_bootstrap : https://github.com/charles-lunarg/vk-bootstrap.

Uses Vulkan Memory Allocator (By AMD) for memory allocation (CPU to GPU, and for buffers in general) \
Link to VMA : https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator

Uses TinyObjLoader to load .obj and associated .mtl files. \
Link : https://github.com/tinyobjloader/tinyobjloader
# Goals in the future
Ui (Integrating DearImGui) \
Custom ECS \
Custom math library (currently using GLM).