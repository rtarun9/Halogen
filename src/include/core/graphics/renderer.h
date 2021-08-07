#pragma once

#include "../window.h"
#include "../platform.h"

#include "../../timer.h"
#include "../../log.h"
#include "../../configuration.h"
#include "../../common.h"

#include "vk_debug.h"
#include "vk_initializers.h"
#include "vk_pipeline.h"
#include "vk_mesh.h"


#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <iostream>
#include <string>
#include <memory>


namespace halogen
{
    struct  QueueFamilyIndices
    {
        uint32_t get_graphics_queue_family()
        {
            return m_graphics_queue_family.value();
        }

        uint32_t get_presentation_queue_family()
        {
            return m_presentation_queue_family.value();
        }

        std::optional<uint32_t> m_graphics_queue_family;
        std::optional<uint32_t> m_presentation_queue_family;
    };

    //Base class for the renderer that ties up all wrapper vulkan / scene classes.
    class Renderer
    {
    public:
        Renderer();
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

        //Pipeline related stuff
        void initialize_pipelines();

        void load_mesh();
        void upload_mesh(Mesh& mesh);

        void clean_up();

    private:

        int m_frame_number {0};
        VkExtent2D m_extent {1080, 720};

        //Core vulkan objects.
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        VkSurfaceKHR m_window_surface;

        //Swapchain related objects.
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        VkQueue m_graphics_queue;
        QueueFamilyIndices m_queue_family_indices;

        VkCommandPool m_command_pool;
        VkCommandBuffer m_command_buffer;

        VkRenderPass m_render_pass;
        std::vector<VkFramebuffer> m_framebuffers;

        //Sync objects.
        VkFence m_render_fence;
        VkSemaphore m_presentation_semaphore;
        VkSemaphore m_render_semaphore;

        //Pipeline related
        PipelineConfig m_pipeline_config;
        VkPipeline m_triangle_pipeline;

        //For clean up
        DeletionQueue m_deletion_queue;

        //Memory allocator so we can do memory allocation
        VmaAllocator m_vma_allocator;

        //Meshes
        Mesh m_triangle_mesh;

    };
}
