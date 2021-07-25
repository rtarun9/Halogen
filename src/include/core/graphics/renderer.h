#ifndef RENDERER_H
#define RENDERER_H

#include "../../log.h"
#include "vk_debug.h"
#include "vk_initializers.h"
#include "../../configuration.h"
#include "../window.h"
#include "../platform.h"

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <iostream>
#include <string>
#include <memory>


namespace halogen
{
    //struct for queue families
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
        void clean_up();

    private:
        int m_frame_number {0};
        VkExtent2D m_extent {720, 680};

        VkInstance m_instance{};
        VkDebugUtilsMessengerEXT m_debug_messenger{};
        VkPhysicalDevice m_physical_device{};
        VkDevice m_device{};
        VkSurfaceKHR m_window_surface{};

        VkSwapchainKHR m_swapchain{};
        VkFormat m_swapchain_image_format;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        VkQueue m_graphics_queue{};
        QueueFamilyIndices m_queue_family_indices;

        VkCommandPool m_command_pool{};
        VkCommandBuffer m_command_buffer{};

        VkRenderPass m_render_pass{};
        std::vector<VkFramebuffer> m_framebuffers;

        VkFence m_render_fence;
        VkSemaphore m_presentation_semaphore;
        VkSemaphore m_render_semaphore;
    };
}
#endif