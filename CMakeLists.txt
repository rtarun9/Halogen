cmake_minimum_required(VERSION 3.15)

project(Halogen)

set(CMAKE_CXX_STANDARD 17)

execute_process(COMMAND bash compile_shader.sh WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
include_directories(${VULKAN_INCLUDE_DIRS})

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

set (VK_MEMORY_ALLOCATOR third_party/vma)
include_directories(${VK_MEMORY_ALLOCATOR})

set (VK_BOOTSTRAP third_party/vk_bootstrap)
include_directories(${VK_BOOTSTRAP})

set (TINY_OBJ_LOADER third_party/tiny_obj_loader)
include_directories(${TINY_OBJ_LOADER})

set(PROJECT_HEADER_FILES
        src/include/core/log.h
        src/include/core/common.h
        src/include/halogen.h
        src/include/core/configuration.h

        src/include/core/internal/timer.h
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

        src/include/core/math/math.h
        src/include/core/math/vector2.h
        src/include/core/math/vector3.h
        src/include/core/math/vector4.h
        src/include/core/math/matrix3x3.h
        src/include/core/math/matrix4x4.h
        src/include/core/math/matrix_transform.h

        src/include/core/game/game_object.h
        src/include/core/game/scene.h
        src/include/core/game/camera.h
)

set(PROJECT_SRC_FILES
        src/source/main.cpp

        src/source/internal/window.cpp
        src/source/internal/input.cpp
        src/source/internal/engine.cpp
        src/source/internal/platform.cpp
        src/source/internal/timer.cpp

        src/source/graphics/renderer.cpp
        src/source/graphics/vk_initializers.cpp
        src/source/graphics/vk_pipeline.cpp
        src/source/graphics/vk_mesh.cpp

        src/source/math/vector2.cpp
        src/source/math/vector3.cpp
        src/source/math/vector4.cpp
        src/source/math/matrix3x3.cpp
        src/source/math/matrix4x4.cpp

        src/source/game/game_object.cpp
        src/source/game/scene.cpp
        src/source/game/camera.cpp
)

set (THIRD_PARTY_FILES
        third_party/vk_bootstrap/vk_bootstrap.h
        third_party/vk_bootstrap/vk_bootstrap_dispatch.h
        third_party/tiny_obj_loader/tiny_obj_loader.h

        third_party/vk_bootstrap/vk_bootstrap.cpp
        third_party/tiny_obj_loader/tiny_obj_loader.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_HEADER_FILES} ${PROJECT_SRC_FILES} ${THIRD_PARTY_FILES})

target_precompile_headers(${PROJECT_NAME} PUBLIC <vk_mem_alloc.h> <vk_bootstrap.h> <vk_bootstrap_dispatch.h> <tiny_obj_loader.h>)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan ${SDL2_LIBRARIES} ${GLM_LIBRARIES} ${CMAKE_DL_LIBS})