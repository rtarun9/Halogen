cmake_minimum_required(VERSION 3.15)

project(halogen)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
include_directories(${VULKAN_INCLUDE_DIRS})

set(SRC_FILES
        src/source/main.cpp
        src/include/core/window.h
        src/source/window.cpp
        src/include/configuration.h
        src/include/core/input.h
        src/source/input.cpp
        src/source/engine.cpp
        src/include/core/engine.h
        src/include/log.h
        src/include/halogen.h
        src/include/core/platform.h
        src/source/platform.cpp
        src/include/core/graphics/renderer.h
        src/source/graphics/renderer.cpp
        src/include/core/graphics/vk_debug.h
        src/include/common.h
        src/include/core/graphics/instance.h
        src/source/graphics/instance.cpp
        src/include/core/graphics/vk_external.h
        src/include/core/graphics/device.h
        src/source/graphics/device.cpp
        src/include/core/graphics/surface.h
        src/include/core/graphics/swapchain.h
        src/source/graphics/swapchain.cpp
        src/source/graphics/surface.cpp)


add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} Vulkan::Vulkan)