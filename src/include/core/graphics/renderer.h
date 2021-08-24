#pragma once

#include "../internal/timer.h"
#include "../log.h"
#include "../configuration.h"
#include "../common.h"

#include "../internal/window.h"
#include "../internal/platform.h"
#include "../internal/input.h"

#include "../math/math.h"

#include "../game/scene.h"
#include "../game/game_object.h"
#include "../game/camera.h"

#include "vk_debug.h"
#include "vk_initializers.h"
#include "vk_pipeline.h"
#include "vk_mesh.h"
#include "vk_types.h"

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace halogen
{
    //Base class for the renderer that ties up all wrapper vulkan / scene classes.
    class Renderer
    {
    public:
        Renderer(Input& input);
        ~Renderer();

        void initialize_renderer(const Window& window);
        void render();

    private:
        void initialize_vulkan(const Window& window);
        void initialize_renderer_core(const Window& window);
        void create_swapchain();
        void create_command_objects();
        void create_render_pass();
        void create_framebuffers();
        void create_sync_objects();

        //For descriptors
        void initialize_descriptors();

        //Pipeline related stuff
        void initialize_pipelines();

        void load_mesh();
        void upload_mesh(Mesh& mesh);
        void initialize_scene();

        //Utilitiy functions
        wrapper::FrameData& get_current_frame();

        void clean_up();

    private:
    	//references to other classes
    	Input& m_input;

    	//Not sure what to name this section.
        int m_frame_number {0};
        VkExtent2D m_extent {1080, 720};

        //Core vulkan objects.
        VkInstance m_instance{};
        VkDebugUtilsMessengerEXT m_debug_messenger{};
        VkPhysicalDevice m_physical_device{};
        VkDevice m_device{};
        VkSurfaceKHR m_window_surface{};

        //Wrappers for vulkan objects.
        wrapper::Swapchain m_swapchain_wrapper{};
        wrapper::DepthImage m_depth_image_wrapper{};
		std::array<wrapper::FrameData, configuration::MAX_FRAMES_IN_FLIGHT> m_frames;

        VkQueue m_graphics_queue{};
        VkQueue m_presentation_queue_extension{};
        QueueFamilyIndices m_queue_family_indices{};

        VkRenderPass m_render_pass{};
        std::vector<VkFramebuffer> m_framebuffers{};

        //Descriptor set related
        VkDescriptorPool m_descriptor_pool{};
        VkDescriptorSetLayout m_global_descriptor_set_layout{};

        //Pipeline related
        PipelineConfig m_pipeline_config{};
        VkPipeline m_primary_pipeline{};

        //For clean up
        DeletionQueue m_deletion_queue{};

        //Memory allocator so we can do memory allocation
        VmaAllocator m_vma_allocator{};

        //Meshes
        Mesh m_triangle_mesh{};
        Mesh m_monkey_mesh{};

        Scene m_main_scene{};
    };
}
