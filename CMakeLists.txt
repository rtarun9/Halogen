cmake_minimum_required(VERSION 3.15)

project(Halogen)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
include_directories(${VULKAN_INCLUDE_DIRS})

#[NOTE]: GLM is used for math now, might use custom math library in the future.
find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

#For VK_BOOTSTRAP libraries
set (VK_BOOTSTRAP third_party/vk_bootstrap)

include_directories(${VK_BOOTSTRAP})

set (VK_MEMORY_ALLOCATOR third_party/vma)
include_directories(${VK_MEMORY_ALLOCATOR})

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
        src/include/common.h
        src/include/core/graphics/vk_external.h
        src/include/core/graphics/vk_initializers.h
        src/source/graphics/vk_initializers.cpp
        src/include/core/graphics/vk_debug.h
        src/include/core/graphics/vk_pipeline.h
        src/source/graphics/vk_pipeline.cpp
        src/include/core/graphics/vk_mesh.h
        src/source/graphics/vk_mesh.cpp
        ${VK_BOOTSTRAP}/vk_bootstrap.cpp
        third_party/vk_bootstrap/vk_bootstrap.h
        third_party/vk_bootstrap/vk_bootstrap_dispatch.h
        src/include/core/graphics/vk_types.h)

add_executable(${PROJECT_NAME} ${SRC_FILES})

##[NOTE] : This is fine because I most probably wont be messing around with the third_party libraries
target_precompile_headers(${PROJECT_NAME} PUBLIC ${VK_MEMORY_ALLOCATOR}/vk_mem_alloc.h)

#dl_libs specifically used for vk_bootstrap (since it loads vulkan on its own using dl_open).
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} Vulkan::Vulkan ${GLM_LIBRARIES} ${CMAKE_DL_LIBS})
