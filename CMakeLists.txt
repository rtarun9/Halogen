cmake_minimum_required(VERSION 3.15)

project("Halogen")

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan)
find_package(SDL2 CONFIG REQUIRED)


add_subdirectory(third_party)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

if(MSVC)
    add_compile_options(/WX)
    add_link_options(/WX)
else()
    add_compile_options(-werror)
endif()

add_subdirectory(src)

find_program(GLSL_VALIDATOR glslangValidator HINTS $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

## find all the shader files under the shaders folder
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
)

# PCH
target_precompile_headers(Halogen 
    PRIVATE 
        <string>
        <vector>
        <iostream>
        <unordered_map>
        <initializer_list>
        <cmath>
        <algorithm>
        <fstream>
        <exception>
     
    PUBLIC
        <vulkan/vulkan.hpp>
)

## iterate each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  message("Building shader ")
  message(STATUS ${GLSL}) 
  ##execute glslang command to compile that specific shader
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    run ALL
    DEPENDS ${SPIRV_BINARY_FILES} 
)