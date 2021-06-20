cmake_minimum_required(VERSION 3.15)

project(halogen)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SRC_FILES
        halogen/src/source/main.cpp
        halogen/src/include/core/window.h
        halogen/src/source/window.cpp
        halogen/src/include/core/configuration.h
        halogen/src/include/core/input.h
        halogen/src/source/input.cpp
        halogen/src/source/engine.cpp
        halogen/src/include/core/engine.h
        halogen/src/include/log.h halogen/src/include/halogen.h)


add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})