# Halogen
A simple and small graphics engine. This project uses SDL2 for windowing / input, and Vulkan for the graphics backend. 
The purpose of making this engine is to : try out graphics programming concepts, learn more about GPU's, and improve my programming skills.

# Building the project
The bin directory contains two scripts. Use the build.sh script to build the project. 
You will need to have a C++ 17 Compiler, Cmake, Glslc (Google's GLSL to Spir-V compiler), and SDL2 installed. 
If you do not want to use find_package() in Cmake, you need to modify CMakeLists and add 
your own paths.

# External libraries
Uses vk_bootstrap for a clean way to initialize core parts of vulkan.
Link to vk_bootstrap : https://github.com/charles-lunarg/vk-bootstrap.


