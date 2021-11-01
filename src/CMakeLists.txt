add_executable(Halogen
 "source/main.cpp"
 "source/engine.cpp"
 "source/vk_initializers.cpp"
 "source/vk_pipeline.cpp")

 
set_property(TARGET Halogen PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Halogen>)

target_include_directories(Halogen PUBLIC ${SDL_INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS}) 

target_link_libraries(Halogen vkbootstrap)
target_link_libraries(Halogen SDL2main SDL2 Vulkan::Vulkan)