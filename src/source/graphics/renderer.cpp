#include "../../include/core/graphics/renderer.h"

namespace halogen
{
    Renderer::Renderer(const Input& input) : m_input(input)
    {
    }

    Renderer::~Renderer()
    {
        clean_up();
    }

    void Renderer::initialize_renderer(const Window& window)
    {
        initialize_vulkan(window);
        initialize_pipelines();
    }

    void Renderer::render(int selected_pipeline)
    {
        //Block CPU until previous GPU command has not executed succesfully.
        vk_check(vkWaitForFences(m_device, 1, &m_render_fence, VK_TRUE, common::Time::One_Second), "Failed waiting for fence. Timeout : 1 second");
        vk_check(vkResetFences(m_device, 1, &m_render_fence));

        uint32_t swapchain_image_index = -1;

        //Wait on the semaphore since you dont want to prepare next frame until GPU execution is done   .
        vk_check(vkAcquireNextImageKHR(m_device, m_swapchain, common::Time::One_Second, m_presentation_semaphore, nullptr, &swapchain_image_index));

        //Begin rendering commands

        vk_check(vkResetCommandBuffer(m_command_buffer, 0));

        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.pNext = nullptr;
        command_buffer_begin_info.pInheritanceInfo = nullptr;

        //Let vulkan know this will command buffer will only be executed / submitted once. (It is being re - recorded every  frame).
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vk_check(vkBeginCommandBuffer(m_command_buffer, &command_buffer_begin_info), "Failed to begin recording into command buffer.");

        VkClearValue clear_value;
        clear_value.color = {0.1f, 0.1f, 0.1f};

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;

        render_pass_begin_info.renderPass = m_render_pass;
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.renderArea.extent = m_extent;

        //Render into the target framebuffer (target framebuffer index : index of swapchain that has been freshly acquired).
        render_pass_begin_info.framebuffer = m_framebuffers[swapchain_image_index];

        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        //not calling vk_check for begin render pass since it return void, and its guaranteed to work.
        //Bind the framebuffer, clear the image, and set image layout to that specified when creation of renderpass happened.
        vkCmdBeginRenderPass(m_command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        //if selected pipeline is 0, do the rgb triangle. if 1, do the plain one.
        if (selected_pipeline == 0)
        {
            vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_rgb_triangle_pipeline);
        }
        else if (selected_pipeline == 1)
        {
            vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_plain_triangle_pipeline);
        }

        //Draw 1 object, with 3 vertices.
        vkCmdDraw(m_command_buffer, 3, 1, 0, 0);

        //Transitions the image layout so that it can be presented, as rendering is done.
        vkCmdEndRenderPass(m_command_buffer);
        vk_check(vkEndCommandBuffer(m_command_buffer));

        //Prepare for submission to queue
        //presentation semaphore : signalled when the swapchain is ready.
        //render semaphore : signals when rendering is complete.

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;

        VkPipelineStageFlags  wait_stages =
        {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        //Wait on the presentation semaphore, since it is signalled when image from swapchain is acquired.
        submit_info.pWaitDstStageMask = &wait_stages;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &m_presentation_semaphore;

        //Signal to the render semaphore about  rendering status.
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &m_render_semaphore;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_command_buffer;

        //Block CPU until graphics commands have finished thier execution.
        vk_check(vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_render_fence), "Failed to submit to queue");

        //Present to screen after commands are submitted
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;

        present_info.pSwapchains = &m_swapchain;
        present_info.swapchainCount = 1;

        //Wait on render semaphore (presentation is blocked off until rendering is complete)
        present_info.pWaitSemaphores = &m_render_semaphore;
        present_info.waitSemaphoreCount = 1;

        present_info.pImageIndices = &swapchain_image_index;

        vk_check(vkQueuePresentKHR(m_graphics_queue, &present_info));

        m_frame_number++;
    }

    void Renderer::initialize_vulkan(const Window& window)
    {
        initialize_renderer_core(window);
        create_swapchain();
        create_command_objects();
        create_render_pass();
        create_framebuffers();
        create_sync_objects();
    }

    //Uses vk_bootstrap to quickly set up some core instances of vulkan (device, instance, etc).
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

        //Pass in the window surface so that physical device selecter will choose GPU that can actually render onto the screen.
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

    //Uses vk_boostrap to create swapchain.
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

    //Create command buffers and command objects.
    void Renderer::create_command_objects()
    {
        //CommandPool : allocator for command buffers.

        //flag used here is to specify that command buffers will be reset using vkResetCommandBuffer. (Its state needs to be reset from pending to initial.
        VkCommandPoolCreateInfo command_pool_create_info = vkinit::command_objects::create_command_pool_create_info(
                m_queue_family_indices.get_graphics_queue_family(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        vk_check(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool), "Failed to create comamnd pool");

        //primary level command buffer, are sent into the Vkqueue's (execution port of GPU).
        //secondary level command buffers, are sub buffers to the to the primary command buffer.
        VkCommandBufferAllocateInfo command_buffer_allocate_info = vkinit::command_objects::create_command_buffer_allocate_info(
                m_command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        vk_check(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_command_buffer), "Failed to allocate command buffer");
    }

    //Creating of renderpass
    void Renderer::create_render_pass()
    {
        //Point of renderpass : store details of the image we are writing into (renderpass writes into the framebuffer, which links to the actual image we are writing into).
        //Also does image layout transition (since images are generally stored in someother format by GPU for optimization).

        //Note : Image layouts :
        //UNDEFINED -> Renderpass begins -> Subpass 0 begins -> Image layout set to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (optimal for writing into image) ->
        // Subpass 0 renders -> Subpass 0 ends -> Renderpass ends -> Image layout set to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.

        //Desciption of image we will be writting into, using the renderpass.
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = m_swapchain_image_format;

        //Set to 1, since there is no MSAA (anti aliasing)
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

        //Clear attachment when loaded, and retain attachment when renderpass ends.
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        //Dont care about stencil operations for now.
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        //Initially, layout is undefined. Then after renderpass ends, image must be able to be rendered on the screen.
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_reference = {};

        //Reference into the render_pass's pAttachment array. layout is set to color optimal since it will be optimal for writing into the color attachment.
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //Only setting up 1 subpass (which is the minimal). Wont be used much in the engine.
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

    //Creation of frame buffers (made according to renderpass, which is created before this).
    void Renderer::create_framebuffers()
    {
        //Framebuffer : Link between the attachments (such as color) and the actual VkImage that is rendered into.
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

    //Fence and Semaphore creation.
    void Renderer::create_sync_objects()
    {
        //Create Fence for GPU - CPU communication.
        VkFenceCreateInfo fence_create_info = {};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.pNext = nullptr;

        //Create fence in the signalled state. Kinda a edge case for the initial time you want to render something, so you can wait on it before doing GPU commands (for the first time).
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        vk_check(vkCreateFence(m_device, &fence_create_info, nullptr, &m_render_fence), "Failed to create fence.");

        //Create semaphores for GPU - GPU communication.
        VkSemaphoreCreateInfo semaphore_create_info = {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_create_info.pNext = nullptr;
        semaphore_create_info.flags = 0;

        vk_check(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_presentation_semaphore), "Failed to create presentation semaphore");
        vk_check(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_render_semaphore), "Failed to create render semaphore");
    }

    //Pipeline related stuff, move later
    void Renderer::initialize_pipelines()
    {
        //Stuff for shader stages (both fragment and vertex shader).

        //Shader stuff for the rgb triangle
        VkShaderModule rgb_vertex_shader_module;
        VkShaderModule rgb_fragment_shader_module;

        pipeline_utils::create_shader_module(m_device, "rgb_triangle_vertex_shader.spv", &rgb_vertex_shader_module);
        pipeline_utils::create_shader_module(m_device, "rgb_triangle_fragment_shader.spv", &rgb_fragment_shader_module);

        //Shader stuff for the plain triangle
        VkShaderModule plain_vertex_shader_module;
        VkShaderModule plain_fragment_shader_module;

        pipeline_utils::create_shader_module(m_device, "plain_triangle_vertex_shader.spv", &plain_vertex_shader_module);
        pipeline_utils::create_shader_module(m_device, "plain_triangle_fragment_shader.spv", &plain_fragment_shader_module);

        //How to read vertex buffers.
        m_pipeline_config.m_vertex_input_stage_info = vkinit::pipeline_objects::create_vertex_input_state_create_info();

        //What do you want to draw.
        m_pipeline_config.m_input_assembly_state_info = vkinit::pipeline_objects::create_input_assembly_state_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        m_pipeline_config.m_viewport.x = 0.0f;
        m_pipeline_config.m_viewport.y = 0.0f;
        m_pipeline_config.m_viewport.width = static_cast<float>(m_extent.width);
        m_pipeline_config.m_viewport.height = static_cast<float>(m_extent.height);
        m_pipeline_config.m_viewport.minDepth = 0.0f;
        m_pipeline_config.m_viewport.maxDepth = 1.0f;

        m_pipeline_config.m_scissor_rectangle.offset = {0, 0};
        m_pipeline_config.m_scissor_rectangle.extent = m_extent;

        //Draw filled rectangles for now..
        m_pipeline_config.m_rasterization_state_create_info = vkinit::pipeline_objects::create_rasterization_state_create_info(VK_POLYGON_MODE_FILL);

        m_pipeline_config.m_multisample_state_create_info = vkinit::pipeline_objects::create_multisample_state_create_info();

        //No blending, but write into RGBA
        m_pipeline_config.m_color_blend_attachment = vkinit::pipeline_objects::create_color_blend_attachment_state();

        //Create and use the pipeline layout.
        VkPipelineLayoutCreateInfo pipeline_layout_create_info =  vkinit::pipeline_objects::create_pipeline_layout_create_info();
        vk_check(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_config.m_layout), "Failed to create pipeline layout");

        //Bind shader stages for the rgb triangle
        m_pipeline_config.m_shader_stages.push_back(vkinit::pipeline_objects::create_pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, rgb_vertex_shader_module));
        m_pipeline_config.m_shader_stages.push_back(vkinit::pipeline_objects::create_pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, rgb_fragment_shader_module));

        //Build pipeline for rgb triangle
        m_rgb_triangle_pipeline = m_pipeline_config.build_pipeline(m_device, m_render_pass);

        //Bind shader stages for the plain triangle
        m_pipeline_config.m_shader_stages.clear();

        m_pipeline_config.m_shader_stages.push_back(vkinit::pipeline_objects::create_pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, plain_vertex_shader_module));
        m_pipeline_config.m_shader_stages.push_back(vkinit::pipeline_objects::create_pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, plain_fragment_shader_module));

        //Build pipeline for rgb triangle
        m_plain_triangle_pipeline = m_pipeline_config.build_pipeline(m_device, m_render_pass);
    }

    void Renderer::clean_up()
    {
        vkDestroyRenderPass(m_device, m_render_pass, nullptr);

        vkDestroyCommandPool(m_device, m_command_pool, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        //No need to destroy images as they are deleted along with the swapchain.
        //Breaking  the rule of "destroy in the reverse order of creation" since framebuffers are created from images.
        for (int i = 0; i < m_swapchain_image_views.size(); i++)
        {
            vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
            vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_window_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);

        debug::warning("For now, none of the pipeline related objects are being cleared up. This is the next thing on the TODO list.");
    }
}