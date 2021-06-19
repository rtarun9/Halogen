cmake_minimum_required(VERSION 3.15)

project(halogen)

set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES ${PROJECT_SOURCE_DIR}/halogen/src/source/main.cpp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC_FILES})