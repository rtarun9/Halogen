# Halogen
A work in progress Graphics engine using C++ / Vulkan. Made for learning / experimenting with computer graphics. 

# Objectives of the engine
* Use Modern C++ (17 and higher) with a focus on writing clean code.
* Learn about Vulkan and CG fundamentals
* Make fancy stuff appear on the screen :)

# Building the project
* Uses Cmake (only SDL2 needs to be vendored in manually).  Automatically compiles vertex and fragment shaders to Spir-V.
* Currently only tested on Windows 10

# Dependencies (Third party)
[SDL2](https://github.com/libsdl-org/SDL) : Windowing and input \
[VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) : Memory allocator for vulkan \
[GLM](https://github.com/g-truc/glm) : Linear algebra math library \
[VK-Bootstrap](https://github.com/charles-lunarg/vk-bootstrap) Handles the boring vulkan setup code

# Resources
[Vk-Guide](https://vkguide.dev/) : Primary resource for vulkan \
[Game-Math](https://gamemath.com/) : Linear algebra 