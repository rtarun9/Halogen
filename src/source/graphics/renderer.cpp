#include "../../include/core/graphics/renderer.h"

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
        create_command_objects();
        create_render_pass();
        create_framebuffers();
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

        //pass in the window surface so that physical device selecter will choose GPU that can actually render onto the screen.
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
    //command pool is a huge allocator for command buffers. A buffer is submitted to a queue, which does the actual work.
    void Renderer::create_command_objects()
    {
        //flag used here is to specify that command buffers are required to be reset if desired.
        VkCommandPoolCreateInfo command_pool_create_info = vkinit::command_objects::command_pool_create_info(m_queue_family_indices.get_graphics_queue_family(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        vk_check(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool), "Failed to create comamnd pool");

        //primary level command buffer, are sent into the Vkqueue's (execution port of GPU).
        //secondary level command buffers, are subbuffers to the to the primary command buffer.
        VkCommandBufferAllocateInfo command_buffer_allocate_info = vkinit::command_objects::command_buffer_allocate_info(m_command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        vk_check(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_command_buffer), "Failed to allocate command buffer");
    }

    //renderpass stores basic details of the state required to set up the "target" for rendering, and the images that will be renderer onto.
    //also renders into a framebuffer, and changes the image layout in required formats.
    void Renderer::create_render_pass()
    {
        //desciption of image we will be writting into, using the renderpass.
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = m_swapchain_image_format;

        //set to 1, since there is no MSAA (anti aliasing)
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

        //clear attachment when loaded, and retain attachment when renderpass ends.
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        //dont care
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        //initially, layout is undefined. Then after renderpass ends, image must be able to be rendered on the screen.
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_reference = {};

        //reference into the render_pass's pAttachment array. layout is set to color optimal since it will be optimal for writing into the color attachment.
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass_description = {};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_attachment_reference;

        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass_description;

        vk_check(vkCreateRenderPass(m_device, &render_pass_create_info, nullptr, &m_render_pass), "Failed to create render pass.");
    }

    //link between renderpass attachments and image view's (from swapchain)
    void Renderer::create_framebuffers()
    {
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.renderPass = m_render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.width = m_extent.width;
        framebuffer_create_info.height = m_extent.height;
        framebuffer_create_info.layers = 1;

        const uint32_t swapchain_image_count = m_swapchain_images.size();
        m_framebuffers.resize(swapchain_image_count);

        for (int i = 0; i < swapchain_image_count; i++)
        {
            framebuffer_create_info.pAttachments = &m_swapchain_image_views[i];
            vk_check(vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffers[i]), "Failed to create framebuffers");
        }
    }

    void Renderer::clean_up()
    {

        vkDestroyRenderPass(m_device, m_render_pass, nullptr);

        vkDestroyCommandPool(m_device, m_command_pool, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        //no need to destroy images as they are deleted along with the swapchain.
        //breaking  the rule of "destroy in the reverse order of creation" since framebuffers are created from images.
        for (int i = 0; i < m_swapchain_image_views.size(); i++)
        {
            vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
            vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_window_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
    }
}