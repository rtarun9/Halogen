#include "renderer.h"

namespace halogen
{
    Renderer::Renderer()
    {
    }

    Renderer::~Renderer()
    {
        clean_up();
    }

    void Renderer::initialize_renderer(const Window& window)
    {
        initialize_vulkan(window);
    }

    void Renderer::render()
    {

    }

    void Renderer::initialize_vulkan(const Window& window)
    {
        initialize_renderer_core(window);
        create_swapchain();
    }

    //uses vk_bootstrap to quickly set up some core instances of vulkan (device, instance, etc).
    void Renderer::initialize_renderer_core(const Window &window)
    {
        vkb::InstanceBuilder builder;
        auto instance = builder.set_app_name("Halogen")
                .request_validation_layers()
                .use_default_debug_messenger()
                .build();

        if (!instance)
        {
            debug::error("Failed to create vulkan instance");
        }

        vkb::Instance vkb_instance = instance.value();
        m_instance = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        Platform::create_window_surface(window.get_window(), m_instance, m_window_surface);

        vkb::PhysicalDeviceSelector device_selector {vkb_instance};
        vkb::PhysicalDevice physical_device = device_selector
                .set_minimum_version(1, 1)
                .set_surface(m_window_surface)
                .select()
                .value();

        vkb::DeviceBuilder device_builder {physical_device};

        vkb::Device vkb_device = device_builder.build().value();

        m_device = vkb_device.device;
        m_physical_device = physical_device.physical_device;

        m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_queue_family_indices.m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    }

    //uses vk_boostrap to create swapchain.
    void Renderer::create_swapchain()
    {
        vkb::SwapchainBuilder swapchain_builder {m_physical_device, m_device, m_window_surface};

        vkb::Swapchain vkb_swapchain = swapchain_builder
                    .use_default_format_selection()
                    .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                    .set_desired_extent(m_extent.width, m_extent.height)
                    .build()
                    .value();

        m_swapchain = vkb_swapchain.swapchain;

        //VkImage is the actual image that is used as texture to render to.
        //VkImageView is a wrapper for the image (allows to do things such as swap colors, etc).

        m_swapchain_images = vkb_swapchain.get_images().value();
        m_swapchain_image_views = vkb_swapchain.get_image_views().value();
        m_swapchain_image_format = vkb_swapchain.image_format;
    }

    //create command pools and command buffers.
    void Renderer::create_command_objects()
    {

    }

    void Renderer::clean_up()
    {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        //no need to destroy images as they are deleted along with the swapchain.
        for (int i = 0; i < m_swapchain_image_views.size(); i++)
        {
            vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_window_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
    }
}