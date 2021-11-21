# Halogen
A work in progress Graphics engine using C++ / Vulkan. Made for learning / experimenting with computer graphics. 

# Objectives of the engine
* Use Modern C++ (17 and higher) with a focus on writing clean code.
* Learn about Vulkan and CG fundamentals
* Make fancy stuff appear on the screen :)

# Building the project
* Uses VCPKG for package management (all third party libs except SDL2 are vendored in). Run the install_packages script according to your OS. Cmake is used for building the project.
It utomatically compiles vertex and fragment shaders to Spir-V.
* Currently only tested on Windows 10 & Windows 11.

# Dependencies (Third party)
[SDL2](https://github.com/libsdl-org/SDL) : Windowing and input \
[VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) : Memory allocator for vulkan \
[VK-Bootstrap](https://github.com/charles-lunarg/vk-bootstrap) Handles the boring vulkan setup code \
[TinyObjLoader](https://github.com/tinyobjloader/tinyobjloader/) For model loading (.obj files only for now)

# Resources
[Vk-Guide](https://vkguide.dev/) : Primary resource for vulkan \
[Game-Math](https://gamemath.com/) : Linear algebra theory