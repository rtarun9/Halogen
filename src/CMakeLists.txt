add_executable(Halogen
 "source/main.cpp"
 "source/engine.cpp"
 "source/initializers.cpp"
 "source/pipeline.cpp"
 "source/mesh.cpp")

set_property(TARGET Halogen PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Halogen>)

target_include_directories(Halogen PUBLIC ${SDL_INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS}) 

target_link_libraries(Halogen vkbootstrap vma tinyobjloader)
target_link_libraries(Halogen SDL2::SDL2 SDL2::SDL2main Vulkan::Vulkan)

target_precompile_headers(Halogen 
	PUBLIC 
	<VkBootstrap.h> 
	<tiny_obj_loader.h> 
	<vk_mem_alloc.h>

	PRIVATE
	"include/custom_math.h"
)