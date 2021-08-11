cmake_minimum_required(VERSION 3.15)

project(Halogen)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
include_directories(${VULKAN_INCLUDE_DIRS})

set (VK_MEMORY_ALLOCATOR third_party/vma)
include_directories(${VK_MEMORY_ALLOCATOR})

set (VK_BOOTSTRAP third_party/vk_bootstrap)
include_directories(${VK_BOOTSTRAP})

set(PROJECT_HEADER_FILES
        src/include/core/log.h
        src/include/core/common.h
        src/include/halogen.h
        src/include/core/configuration.h
        src/include/core/timer.h

        src/include/core/internal/window.h
        src/include/core/internal/input.h
        src/include/core/internal/engine.h
        src/include/core/internal/platform.h

        src/include/core/graphics/renderer.h
        src/include/core/graphics/vk_external.h
        src/include/core/graphics/vk_debug.h
        src/include/core/graphics/vk_pipeline.h
        src/include/core/graphics/vk_mesh.h
        src/include/core/graphics/vk_initializers.h
        src/include/core/graphics/vk_types.h

        src/include/core/math/vector3.h
        src/include/core/math/vector4.h
)


set(PROJECT_SRC_FILES
        src/source/main.cpp
        src/source/internal/window.cpp
        src/source/internal/input.cpp
        src/source/internal/engine.cpp
        src/source/internal/platform.cpp
        src/source/timer.cpp

        src/source/graphics/renderer.cpp
        src/source/graphics/vk_initializers.cpp
        src/source/graphics/vk_pipeline.cpp
        src/source/graphics/vk_mesh.cpp

        src/source/math/vector3.cpp
        src/source/math/vector4.cpp
)

set (THIRD_PARTY_FILES
        third_party/vk_bootstrap/vk_bootstrap.h
        third_party/vk_bootstrap/vk_bootstrap_dispatch.h

        third_party/vk_bootstrap/vk_bootstrap.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_HEADER_FILES} ${PROJECT_SRC_FILES} ${THIRD_PARTY_FILES})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${VK_MEMORY_ALLOCATOR}/vk_mem_alloc.h ${VK_BOOTSTRAP}/vk_bootstrap.h ${VK_BOOTSTRAP}/vk_bootstrap_dispatch.h)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan ${SDL2_LIBRARIES} ${GLM_LIBRARIES} ${CMAKE_DL_LIBS})

