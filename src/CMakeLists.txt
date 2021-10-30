add_executable(
	Halogen
 "source/main.cpp"
 "source/engine.cpp"
 "source/vk_initializers.cpp" "include/utils.h")

target_include_directories(Halogen PUBLIC ${SDL_INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS}) 

target_link_libraries(Halogen SDL2main SDL2)