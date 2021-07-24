cmake_minimum_required(VERSION 3.15)

project(Halogen)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
include_directories(${VULKAN_INCLUDE_DIRS})

set(VK_BOOTSTRAP
        third_party/vk_bootstrap.h
        third_party/vk_bootstrap.cpp
        third_party/vk_bootstrap_dispatch.h)

set(SRC_FILES
        src/main.cpp
        src/core/window.h
        src/core/window.cpp
        src/configuration.h
        src/core/input.h
        src/core/input.cpp
        src/core/engine.cpp
        src/core/engine.h
        src/log.h
        src/halogen.h
        src/core/platform.h
        src/core/platform.cpp
        src/core/graphics/renderer.h
        src/core/graphics/renderer.cpp
        src/common.h
        src/core/graphics/vk_external.h
        src/core/graphics/vk_initializers.h
        src/core/graphics/vk_initializers.cpp
        src/core/graphics/vk_debug.h
        ${VK_BOOTSTRAP})

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} Vulkan::Vulkan ${CMAKE_DL_LIBS})