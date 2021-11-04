set(SDL_INCLUDE_DIR "C:/Libs/SDL2-2.0.16/include")
set(GLM_INCLUDE_DIR "C:/Libs/glm")

add_executable(Halogen
 "source/main.cpp"
 "source/engine.cpp"
 "source/initializers.cpp"
 "source/pipeline.cpp"
 "source/mesh.cpp")

 
set_property(TARGET Halogen PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Halogen>)

target_include_directories(Halogen PUBLIC ${SDL_INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS} ${GLM_INCLUDE_DIR}) 

target_link_libraries(Halogen vkbootstrap vma tinyobjloader)
target_link_libraries(Halogen SDL2main SDL2 Vulkan::Vulkan)