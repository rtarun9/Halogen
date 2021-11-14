find_package(Vulkan REQUIRED)

add_library(tinyobjloader STATIC)
add_library(vkbootstrap STATIC)
add_library(vma INTERFACE)

target_sources(vkbootstrap PRIVATE
	vkbootstrap/VkBootstrap.h
	vkbootstrap/VkBootstrap.cpp
)

target_sources(tinyobjloader PRIVATE
    tinyobjloader/tiny_obj_loader.h 
	tinyobjloader/tiny_obj_loader.cpp
)

target_include_directories(tinyobjloader PUBLIC tinyobjloader)
target_include_directories(vkbootstrap PUBLIC vkbootstrap)
target_include_directories(vma INTERFACE vma)

target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan $<$<BOOL:UNIX>:${CMAKE_DL_LIBS}>)
