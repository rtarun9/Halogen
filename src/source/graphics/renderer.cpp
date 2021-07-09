#include "../../include/core/graphics/renderer.h"

namespace halogen
{
    /* Core Renderer functions. */
    Renderer::Renderer(std::shared_ptr<Window> &window_reference) : m_window_reference(window_reference),
        m_device(VK_NULL_HANDLE),
        m_window_surface(VK_NULL_HANDLE),
        m_vulkan_instance(VK_NULL_HANDLE),
        m_physical_device(VK_NULL_HANDLE),
        m_swapchain(VK_NULL_HANDLE),
        m_debug_messenger(VK_NULL_HANDLE),
        m_presentation_queue(VK_NULL_HANDLE),
        m_graphics_queue(VK_NULL_HANDLE)
    {
    }

    void Renderer::initialize_renderer()
    {
        initialize_vulkan();
    }

    void Renderer::render()
    {
//        uint32_t image_index;
//        vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_image_available_semaphore, VK_NULL_HANDLE, &image_index);
//
//        VkSubmitInfo submit_info = {};
//        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//        VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        submit_info.waitSemaphoreCount = 1;
//        submit_info.pWaitSemaphores = &m_image_available_semaphore;
//        submit_info.pWaitDstStageMask = &wait_stages;
//        submit_info.commandBufferCount = 1;
//        submit_info.pCommandBuffers = &m_command_buffers[image_index];
//
//        submit_info.signalSemaphoreCount = 1;
//        submit_info.pSignalSemaphores = &m_render_finished_semaphore;
//
//        VK_CHECK(vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE));
//
//        VkPresentInfoKHR  present_info = {};
//        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//        present_info.waitSemaphoreCount = 1;
//        present_info.pWaitSemaphores = &m_render_finished_semaphore;
//
//        present_info.pSwapchains = &m_swapchain;
//        present_info.swapchainCount = 1;
//        present_info.pImageIndices = &image_index;
//
//        vkQueuePresentKHR(m_presentation_queue, &present_info);
    }

    void Renderer::cleanup()
    {
        if (configuration::VALIDATION_LAYERS)
        {
            destroy_debug_utils_messenger_ext(m_vulkan_instance, m_debug_messenger, nullptr);
        }

        /* VkImagesViews are not automatically destroyed like VkImages, so do it manually. */
        for (const VkImageView image_view : m_swapchain_image_views)
        {
            vkDestroyImageView(m_device, image_view, nullptr);
        }

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        vkDestroySurfaceKHR(m_vulkan_instance, m_window_surface, nullptr);


        for(const VkFramebuffer framebuffer : m_swapchain_framebuffers)
        {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        vkDestroyRenderPass(m_device, m_render_pass, nullptr);

        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_vulkan_instance, nullptr);
    }

    Renderer::~Renderer()
    {
        cleanup();
    }

    /* Vulkan related core functions */
    void Renderer::initialize_vulkan()
    {
        create_vulkan_instance();
        setup_debug_messenger();
        create_window_surface();
        select_physical_device();
        create_device();
        create_swapchain();
        create_swapchain_image_views();
        create_render_pass();
        create_graphics_pipeline();
        create_framebuffers();
        create_command_pool();
        create_command_buffers();
        create_sync_objects();
    }

    void Renderer::create_vulkan_instance()
    {
        if (configuration::DEBUG && !check_validation_layer_support())
        {
            debug::error("Failed to find all validation layers");
        }

        /* apiVersion must be the highest version of vulkan that this engine is designed to use.
         * Major and Minor versions of the instance must match those requested in apiVersion.
         * The minimum version must be atleast 1.1, because if so application can use various versions of the api. */
        VkApplicationInfo application_info = {};
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pNext = nullptr;
        application_info.pApplicationName = "halogen";
        application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.pEngineName = "halogen engine";
        application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.apiVersion = VK_API_VERSION_1_2;

        std::vector<const char*> instance_extension_names;
        uint32_t instance_extension_count = 0;

        get_instance_extensions(instance_extension_names, instance_extension_count);

        /* Debug messenger specifically for instance creation and destruction. */
        VkDebugUtilsMessengerCreateInfoEXT create_info_debug_messenger;
        construct_debug_utils_messenger_create_info(create_info_debug_messenger, debug_callback);

        VkInstanceCreateInfo instance_create_info = {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.flags = 0;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledExtensionCount = instance_extension_count;
        instance_create_info.ppEnabledExtensionNames = instance_extension_names.data();

        if (configuration::DEBUG)
        {
            instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &create_info_debug_messenger;
            instance_create_info.enabledLayerCount = configuration::VALIDATION_LAYER_COUNT;
            instance_create_info.ppEnabledLayerNames = configuration::VALIDATION_LAYERS;
        }
        else
        {
            instance_create_info.pNext = nullptr;
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.ppEnabledLayerNames = nullptr;
        }

        VkResult instance_creation_result = vkCreateInstance(&instance_create_info, nullptr, &m_vulkan_instance);
        if (instance_creation_result != VK_SUCCESS)
        {
            VK_CHECK(instance_creation_result);
            debug::error("Failed to create vulkan instance.");
        }
    }

    void Renderer::setup_debug_messenger()
    {
        if (!configuration::DEBUG)
        {
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {};
        construct_debug_utils_messenger_create_info(debug_utils_messenger_create_info,
                                                             (debug_callback));

        VkResult debug_utils_messenger_creation_result = create_debug_utils_messenger_EXT(m_vulkan_instance, &debug_utils_messenger_create_info, nullptr, &m_debug_messenger);
        if (debug_utils_messenger_creation_result != VK_SUCCESS)
        {
            VK_CHECK(debug_utils_messenger_creation_result);
            debug::error("Failed to create debug utils messenger");
        }
    }

    /* Included before physical device selection as window surface may influence the physical device selection process. */
    void Renderer::create_window_surface()
    {
        VkResult window_surface_creation_result = create_platform_specific_window_surface();
        if (window_surface_creation_result != VK_SUCCESS)
        {
            VK_CHECK(window_surface_creation_result);
            debug::error("Could not create window surface.");
        }
    }

    void Renderer::select_physical_device()
    {
        std::vector<VkPhysicalDevice> available_physical_devices;
        uint32_t available_physical_device_count = 0;

        get_supported_physical_devices(available_physical_devices, available_physical_device_count);

        for (VkPhysicalDevice& physical_device : available_physical_devices)
        {
            if (is_physical_device_suitable(physical_device))
            {
                /* Note : this favours GPU that is in the later slots of available_physical_devices vector .*/
                m_physical_device = physical_device;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE)
        {
            debug::error("Failed to select any suitable physical devices.");
        }

        vk_debug::display_physical_device_details(m_physical_device);
    }


    void Renderer::create_device()
    {
        QueueFamilyIndices queue_family_indices = get_queue_family_indices(m_physical_device);
        float queue_priority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
        std::set<uint32_t> unique_queue_families;
        unique_queue_families.insert(queue_family_indices.m_graphics_queue_family.value());
        unique_queue_families.insert(queue_family_indices.m_presentation_queue_family.value());

        for (uint32_t queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo device_queue_create_info = {};
            device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_info.pNext = nullptr;
            device_queue_create_info.queueFamilyIndex = queue_family;
            device_queue_create_info.queueCount = 1;
            device_queue_create_info.pQueuePriorities = &queue_priority;
            device_queue_create_infos.push_back(device_queue_create_info);
        }

        /* None for now. */
        VkPhysicalDeviceFeatures physical_device_features = {};

        VkDeviceCreateInfo device_create_info = {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = nullptr;
        device_create_info.flags = 0;
        device_create_info.queueCreateInfoCount = device_queue_create_infos.size();
        device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
        device_create_info.pEnabledFeatures = &physical_device_features;

        if (configuration::DEBUG)
        {
            /* Not really necessary, since newer vulkan api's dont distinguish between instance / device specific validation layers. */
            device_create_info.ppEnabledLayerNames = configuration::VALIDATION_LAYERS;
            device_create_info.enabledLayerCount = configuration::VALIDATION_LAYER_COUNT;
        }
        else
        {
            device_create_info.ppEnabledLayerNames = nullptr;
            device_create_info.enabledLayerCount = 0;
        }

        device_create_info.enabledExtensionCount = configuration::DEVICE_EXTENSION_COUNT;
        device_create_info.ppEnabledExtensionNames = configuration::DEVICE_EXTENSIONS;

        VkResult device_creation_result = vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device);
        if (device_creation_result != VK_SUCCESS)
        {
            VK_CHECK(device_creation_result);
            debug::error("Failed to create device.");
        }

        vkGetDeviceQueue(m_device, queue_family_indices.m_graphics_queue_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, queue_family_indices.m_presentation_queue_family.value(), 0, &m_presentation_queue);
    }

    void Renderer::create_swapchain() {
        SwapchainDetails swapchain_details = get_swapchain_support_details(m_physical_device);

        m_swapchain_surface_format = select_preferred_swapchain_surface_format(swapchain_details.m_surface_formats);
        m_swapchain_presentation_mode = select_preferred_swapchain_presentation_mode(swapchain_details.m_presentation_modes);
        m_swapchain_extent = select_swapchain_extent(swapchain_details.m_surface_capabilites);

        /* Initially image_count is set to minImageCount + 1 since the driver might take time
         * to acquire image to render to.
         * If maxImageCount is 0, it means no maximum value for image count has been specified. */
        uint32_t image_count = swapchain_details.m_surface_capabilites.minImageCount + 1;
        if (image_count > swapchain_details.m_surface_capabilites.maxImageCount &&
            swapchain_details.m_surface_capabilites.maxImageCount > 0) {
            image_count = swapchain_details.m_surface_capabilites.maxImageCount;
        }

        /* imageUsage is set to VK_IMAGE_USAGE_COLOR_BUFFER_BIT since rendering happens directly on swapchain images, so it acts a a color attachment. */
        VkSwapchainCreateInfoKHR swapchain_create_info = {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.pNext = nullptr;
        swapchain_create_info.flags = 0;
        swapchain_create_info.surface = m_window_surface;
        swapchain_create_info.minImageCount = image_count;
        swapchain_create_info.imageFormat = m_swapchain_surface_format.format;
        swapchain_create_info.imageColorSpace = m_swapchain_surface_format.colorSpace;
        swapchain_create_info.imageExtent = m_swapchain_extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = get_queue_family_indices(m_physical_device);

        uint32_t queue_family_indices[] =
        {
                indices.m_presentation_queue_family.value(),
                indices.m_graphics_queue_family.value()
        };
        int queue_family_indices_count = sizeof(queue_family_indices) / sizeof(uint32_t);

        std::set<uint32_t> unique_queue_family_indices;
        unique_queue_family_indices.insert(indices.m_graphics_queue_family.value());
        unique_queue_family_indices.insert(indices.m_presentation_queue_family.value());

        /* queueFamilyIndexCount is the number of queue's having concurrent access to the swapchain. */
        if (unique_queue_family_indices.size() != queue_family_indices_count)
        {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchain_create_info.queueFamilyIndexCount = 0;
            swapchain_create_info.pQueueFamilyIndices = nullptr;
        }
        else
        {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.queueFamilyIndexCount = queue_family_indices_count;
            swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
        }

        /* compositeAlpha is set as VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR as I dont want the engine window to interact with the other windows in the window system. */
        swapchain_create_info.preTransform = swapchain_details.m_surface_capabilites.currentTransform;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode = m_swapchain_presentation_mode;
        swapchain_create_info.clipped = VK_TRUE;
        swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

        VkResult swapchain_creation_result = vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr,
                                                                  &m_swapchain);
        if (swapchain_creation_result != VK_SUCCESS)
        {
            VK_CHECK(swapchain_creation_result);
            debug::error("Could not create swapchain.");
        }

        /* Acquiring swapchain images. */
        uint32_t swapchain_image_count = 0;
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchain_image_count, nullptr);
        m_swapchain_images.resize(swapchain_image_count);

        vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchain_image_count, m_swapchain_images.data());
    }

    void Renderer::create_swapchain_image_views()
    {
        m_swapchain_image_views.resize(m_swapchain_images.size());

        /* Use image views to specify which part of image to access, and how to access them. */
        for (size_t i = 0; i < m_swapchain_images.size(); i++)
        {
            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = nullptr;
            image_view_create_info.image = m_swapchain_images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = m_swapchain_surface_format.format;

            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_R;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_G;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_B;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_A;

            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            VkResult create_image_view_result = vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_swapchain_image_views[i]);
            if (create_image_view_result != VK_SUCCESS)
            {
                VK_CHECK(create_image_view_result);
                debug::error("Failed to create swap chain image view's.");
            }
        }
    }

    void Renderer::create_render_pass()
    {
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = m_swapchain_surface_format.format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_reference = {};
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_reference;

        VkSubpassDependency dependency;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &dependency;

        VkResult render_pass_creation_result = vkCreateRenderPass(m_device, &render_pass_create_info, nullptr, &m_render_pass);
        if (render_pass_creation_result != VK_SUCCESS)
        {
            VK_CHECK(render_pass_creation_result);
            debug::error("Failed to create render pass.");
        }
    }

    void Renderer::create_graphics_pipeline()
    {
        std::pair<VkShaderModule, VkShaderModule> shader_module = create_shader_modules("vertex_shader.spv", "fragment_shader.spv");
        VkShaderModule vertex_shader_module = shader_module.first;
        VkShaderModule fragment_shader_module = shader_module.second;

        VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {};
        vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_create_info.pNext = nullptr;
        vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_create_info.module = vertex_shader_module;
        vertex_shader_stage_create_info.pName = "main";
        vertex_shader_stage_create_info.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {};
        fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_create_info.pNext = nullptr;
        fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        fragment_shader_stage_create_info.module = fragment_shader_module;
        fragment_shader_stage_create_info.pName = "main";
        fragment_shader_stage_create_info.pSpecializationInfo = nullptr;

        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos =
        {
            vertex_shader_stage_create_info,
            fragment_shader_stage_create_info
        };

        /* This structure defines format of vertex data that will be passed to vertex shader (bindings and descriptions). */
        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
        vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state_create_info.pNext = nullptr;
        vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
        vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
        vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
        vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

        /* Specify type of geometry to draw and if primitive restart should be enabled (so you can break up lines in strip topology by using special indeces . */
        VkPipelineInputAssemblyStateCreateInfo vertex_input_assembly_state_create_info = {};
        vertex_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        vertex_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        vertex_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

        /* Specify region of framebuffer output will be rendered to. */
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchain_extent.width);
        viewport.height = static_cast<float>(m_swapchain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        /* viewport -> transformation from image to framebuffer, and scissor -> which regions pixel will actually be stored. */
        VkRect2D scissor_rectangle = {};
        scissor_rectangle.offset.x = 0.0f;
        scissor_rectangle.offset.y = 0.0f;
        scissor_rectangle.extent = m_swapchain_extent;

        VkViewport viewports[] =
        {
            viewport
        };

        VkRect2D scissor_rectangles[] =
        {
            scissor_rectangle
        };

        /* Viewport -> combination of viewport and scissor. */
        VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = viewports;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = scissor_rectangles;

        /* Rasterizer struct -> takes vertices from vertex shader and turns it into fragments that are colored by fragment shaders. */
        VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = {};
        pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
        pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        pipeline_rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        pipeline_rasterization_state_create_info.lineWidth = 1.0f;
        pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

        /* Multisampling : for AA. set to default by now. */
        VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = {};
        pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
        pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        /* Color blending (after framebuffer). */
        VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {};
        pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                               VK_COLOR_COMPONENT_G_BIT |
                                                               VK_COLOR_COMPONENT_B_BIT |
                                                               VK_COLOR_COMPONENT_A_BIT;
        pipeline_color_blend_attachment_state.blendEnable = VK_FALSE;

        /* ColorBlend  state create info : array of structures for all framebuffers. */
        VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {};
        pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
        pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
        pipeline_color_blend_state_create_info.attachmentCount = 1;
        pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;

        /* Dynamic states : can be changed without recreation of the entire graphics pipeline. */
        VkDynamicState dynamic_states[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info = {};
        pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipeline_dynamic_state_create_info.dynamicStateCount = 2;
        pipeline_dynamic_state_create_info.pDynamicStates = dynamic_states;

        /*  For pipeline layouts (for using uniform variables). */
        VkPipelineLayoutCreateInfo  pipeline_layout_create_info = {};
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkResult pipeline_layout_creation = vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout);
        if (pipeline_layout_creation != VK_SUCCESS)
        {
            debug::error("Could not create pipeline layout. ");
        }

        VkGraphicsPipelineCreateInfo pipeline_create_info = {};
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.stageCount = 1;
        pipeline_create_info.pStages = shader_stage_create_infos.data();
        pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
        pipeline_create_info.pInputAssemblyState = &vertex_input_assembly_state_create_info;
        pipeline_create_info.pViewportState = &viewport_state_create_info;
        pipeline_create_info.pRasterizationState = &pipeline_rasterization_state_create_info;
        pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;
        pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
        pipeline_create_info.layout = m_pipeline_layout;
        pipeline_create_info.renderPass = m_render_pass;
        pipeline_create_info.subpass = 0;

        VkResult graphics_pipeline_creation_result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_graphics_pipeline);
        if (graphics_pipeline_creation_result != VK_SUCCESS)
        {
            VK_CHECK(graphics_pipeline_creation_result);
            debug::error("Could not create graphics pipeline.");
        }

        /* Deleted in this code block as compilation of spir-v to machine code happens after  graphics pipeline is created. */
        vkDestroyShaderModule(m_device, vertex_shader_module, nullptr);
        vkDestroyShaderModule(m_device, fragment_shader_module, nullptr);
    }

    void Renderer::create_framebuffers()
    {
        m_swapchain_framebuffers.reserve(m_swapchain_image_views.size());

        for (size_t i = 0; i < m_swapchain_image_views.size(); i++)
        {
            VkImageView framebuffer_attachments[] =
            {
                m_swapchain_image_views[i]
            };

            VkFramebufferCreateInfo framebuffer_create_info = {};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.renderPass = m_render_pass;
            framebuffer_create_info.attachmentCount = 1;
            framebuffer_create_info.pAttachments = framebuffer_attachments;
            framebuffer_create_info.width = m_swapchain_extent.width;
            framebuffer_create_info.height = m_swapchain_extent.height;
            framebuffer_create_info.layers = 1;

            VK_CHECK(vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_swapchain_framebuffers[i]));
        }
    }

    void Renderer::create_command_pool()
    {
        QueueFamilyIndices queue_family_indices = get_queue_family_indices(m_physical_device);

        VkCommandPoolCreateInfo command_pool_create_info = {};
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.queueFamilyIndex = queue_family_indices.m_graphics_queue_family.value();
        command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool));
    }

    void Renderer::create_command_buffers()
    {
        m_command_buffers.resize(m_swapchain_images.size());

        VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool = m_command_pool;
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t> (m_command_buffers.size());

        VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, m_command_buffers.data()));

        //record command buffer
        for (size_t i = 0; i < m_command_buffers.size(); i++)
        {
            VkCommandBufferBeginInfo command_buffer_begin_info = {};
            command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            VK_CHECK(vkBeginCommandBuffer(m_command_buffers[i], &command_buffer_begin_info));

            VkRenderPassBeginInfo render_pass_begin_info = {};
            render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_begin_info.renderPass = m_render_pass;
            render_pass_begin_info.framebuffer = m_swapchain_framebuffers[i];

            render_pass_begin_info.renderArea.offset = {0, 0};
            render_pass_begin_info.renderArea.extent = m_swapchain_extent;

            VkClearValue clear_value = {0.0, 0.0f, 0.0f, 0.0f};
            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.pClearValues = &clear_value;

            vkCmdBeginRenderPass(m_command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
            vkCmdDraw(m_command_buffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(m_command_buffers[i]);

            vkEndCommandBuffer(m_command_buffers[i]);
        }
    }

    void Renderer::create_sync_objects()
    {
        VkSemaphoreCreateInfo  semaphore_create_info = {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_image_available_semaphore));

        VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_render_finished_semaphore));
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debug_callback
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *user_data
    )
    {
        if (message_severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            std::cerr << callback_data->pMessage << "[OBJECTS : ] : "
                      << callback_data->pObjects << callback_data->sType << '\n';
        }
        return VK_FALSE;
    }

    /* Vulkan helper functions. */
    void Renderer::check_instance_extension_support(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count)
    {
        uint32_t available_extension_count = 0;
        std::vector<VkExtensionProperties> available_extensions;

        vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr);
        available_extensions.resize(available_extension_count);

        vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_extensions.data());

        for (const char * required_instance_extensions : instance_extension_names)
        {
            bool extension_present = false;
            for (const VkExtensionProperties& available_extension_properties : available_extensions)
            {
                if (std::strcmp(required_instance_extensions, available_extension_properties.extensionName) == 0)
                {
                    extension_present = true;
                }
            }

            if (!extension_present)
            {
                debug::error("Could not find all required extensions.");
            }
        }

        vk_debug::display_available_extension(available_extensions);
    }

    void Renderer::get_instance_extensions(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count)
    {
        SDL_Vulkan_GetInstanceExtensions(nullptr, &instance_extension_count, nullptr);
        instance_extension_names.resize(instance_extension_count);

        SDL_Vulkan_GetInstanceExtensions(nullptr, &instance_extension_count, instance_extension_names.data());
        if (configuration::DEBUG)
        {
            instance_extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            instance_extension_count++;
        }

        check_instance_extension_support(instance_extension_names, instance_extension_count);
    }

    /* [NOTE] : Apparently both instance and device extension properties are stored in the same data type, which is VkExtensionProperites.
     * The enumeration function looks very similar as well - so look out for that. */
    bool Renderer::check_device_extension_support(VkPhysicalDevice &device)
    {
        std::vector<VkExtensionProperties> physical_device_features;
        uint32_t physical_device_feature_count;

        vkEnumerateDeviceExtensionProperties(device, nullptr, &physical_device_feature_count, nullptr);
        physical_device_features.resize(physical_device_feature_count);

        vkEnumerateDeviceExtensionProperties(device, nullptr, &physical_device_feature_count, physical_device_features.data());

        for (const char* required_device_extension : configuration::DEVICE_EXTENSIONS)
        {
            bool extension_found = false;
            for (const VkExtensionProperties device_extension : physical_device_features)
            {
                if (strcmp(device_extension.extensionName, required_device_extension) == 0)
                {
                    extension_found = true;
                }
            }

            if (!extension_found)
            {
                return false;
            }
        }

        return true;
    }

    VkResult Renderer::create_platform_specific_window_surface()
    {
        bool window_surface_creation_result = SDL_Vulkan_CreateSurface(&m_window_reference->get_window(), m_vulkan_instance, &m_window_surface);
        if (window_surface_creation_result)
        {
            return VK_SUCCESS;
        }
        else
        {
            return VK_ERROR_SURFACE_LOST_KHR;
        }
    }

    bool Renderer::check_validation_layer_support()
    {
        if (!configuration::DEBUG)
            return false;

        std::vector<VkLayerProperties> available_validation_layers;
        uint32_t available_validation_layer_count = 0;

        vkEnumerateInstanceLayerProperties(&available_validation_layer_count, nullptr);
        available_validation_layers.resize(available_validation_layer_count);

        vkEnumerateInstanceLayerProperties(&available_validation_layer_count, available_validation_layers.data());

        for (const char *requested_validation_layer : configuration::VALIDATION_LAYERS)
        {
            bool layer_found = false;
            for (const VkLayerProperties& available_layer : available_validation_layers)
            {
                if (strcmp(requested_validation_layer, available_layer.layerName) == 0)
                {
                    layer_found = true;
                }
            }

            if (!layer_found)
            {
                debug::error("Could not find all requested validation layers");
                return false;
            }

            vk_debug::display_available_layers(available_validation_layers);
        }

        return true;
    }

    void Renderer::construct_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback)
    {
        create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.pNext = nullptr;
        create_info.flags = 0;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        create_info.pfnUserCallback = debug_callback;
        create_info.pUserData = this;
    }

    /* Pointer to vulkan functions. */
    VkResult Renderer::create_debug_utils_messenger_EXT
    (
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *create_info,
        const  VkAllocationCallbacks *allocator,
        VkDebugUtilsMessengerEXT *messenger
    )
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func == nullptr)
        {
            debug::error("Could not load vkCreateDebugUtilsMessengerEXT");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        else
        {
            return func(instance, create_info, allocator, messenger);
        }
    }

    void Renderer::get_supported_physical_devices(std::vector<VkPhysicalDevice>& available_physical_devices, uint32_t& available_physical_device_count)
    {
        vkEnumeratePhysicalDevices(m_vulkan_instance, &available_physical_device_count, nullptr);
        available_physical_devices.resize(available_physical_device_count);

        VkResult physical_device_enumeration_result = vkEnumeratePhysicalDevices(m_vulkan_instance, &available_physical_device_count, available_physical_devices.data());

        if (available_physical_device_count == 0)
        {
            VK_CHECK(physical_device_enumeration_result);
            debug::error("Could not find any supported vulkan Gpu's.");
        }
    }

    bool Renderer::is_physical_device_suitable(VkPhysicalDevice& physical_device)
    {
        QueueFamilyIndices queue_family_index = get_queue_family_indices(physical_device);

        SwapchainDetails swapchain_support_details = get_swapchain_support_details(physical_device);
        bool physical_device_extension_support = check_device_extension_support(physical_device);

        bool t=  swapchain_support_details.is_swapchain_supported();
        return queue_family_index.is_queue_family_complete() && physical_device_extension_support && swapchain_support_details.is_swapchain_supported();
    }

    QueueFamilyIndices Renderer::get_queue_family_indices(VkPhysicalDevice &physical_device)
    {
        QueueFamilyIndices queue_family_indices;
        uint32_t queue_family_indices_count = 0;

        std::vector<VkQueueFamilyProperties> queue_families;

        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_indices_count, nullptr);
        queue_families.resize(queue_family_indices_count);

        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_indices_count, queue_families.data());

        /* Note : According to the specs, all commands that are allowed on a transfer queue, are also
         * allowed on the graphics / compute queue. So, no need to check for transfer queue. */

        uint32_t current_queue_index = 0;
        for (const VkQueueFamilyProperties queue_family : queue_families)
        {
            /* Check if Graphics Queue is present. */
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                queue_family_indices.m_graphics_queue_family = current_queue_index;
            }

            VkBool32 presentation_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, current_queue_index,  m_window_surface, &presentation_support);
            if (presentation_support == VK_TRUE)
            {
                queue_family_indices.m_presentation_queue_family = current_queue_index;
            }

            if (queue_family_indices.is_queue_family_complete())
            {
                break;
            }

            current_queue_index++;
        }

        return queue_family_indices;
    }

    SwapchainDetails Renderer::get_swapchain_support_details(VkPhysicalDevice &physical_device)
    {
        SwapchainDetails swapchain_support_details = {};

        /* Querying physical device surface capabilities to get details of basic surface features, such as :
         * minimum / maximum swap chain image count, extent of surface, etc. */

        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_window_surface, &swapchain_support_details.m_surface_capabilites));

        /* Query the supported swap chain format-color pairs. */
        uint32_t surface_format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_window_surface, &surface_format_count, nullptr);
        swapchain_support_details.m_surface_formats.resize(surface_format_count);

        if (surface_format_count != 0)
        {
            VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_window_surface, &surface_format_count, swapchain_support_details.m_surface_formats.data()));
        }

        /* Query the swap chain presentation mode. */
        uint32_t presentation_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_window_surface, &presentation_mode_count, nullptr);

        swapchain_support_details.m_presentation_modes.resize(presentation_mode_count);

        if (presentation_mode_count != 0)
        {
            VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_window_surface, &presentation_mode_count, swapchain_support_details.m_presentation_modes.data()));
        }

        return swapchain_support_details;
    }

    /* Helper functions for choosing optimal / fallback swapchain settings. */
    VkSurfaceFormatKHR Renderer::select_preferred_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& availabe_surface_formats)
    {
        /* Choosing RGBA format and color space for now.
         * VK_FORMAT_R8G8B8A8_SRGB is a 3 component 24 bit unsigned normalized format for 8bit R, G, B, A. */
        for (const VkSurfaceFormatKHR& surface_format : availabe_surface_formats)
        {
            if (surface_format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && surface_format.format == VK_FORMAT_R8G8B8A8_SRGB)
            {
                return surface_format;
            }
        }

        /* If preffered swapchain surface format is not available use anyother fallback for it. */
        return availabe_surface_formats[0];
    }

    VkPresentModeKHR Renderer::select_preferred_swapchain_presentation_mode(const std::vector<VkPresentModeKHR>& available_presentation_modes)
    {
        /* Different presentation modes :
         * VK_PRESENT_MODE_IMMEDIATE : image is transferred to screen immediately, very bad tearning.
         * VK_PRESENT_MODE_FIFO_KHR  : image is transferred only at display refresh. If image not ready, waits for next refresh cycle. (V-SYNC).
         * VK_PRESENT_MODE_FIFO_RELAXED_KHR : image is transferred only at display refresh, but if missed transfer image right away. Some visible tearing, low latency.
         * VK_PRESENT_MODE_MAILBOX_KHR      : low latency, no tearing, multiple frame buffers (implements triple buffering).
         * Implementing a hierarchical model here.*/

        bool present_mode_mailbox_found = false;
        bool present_mode_fifo_relaxed_found = false;

        for (const VkPresentModeKHR& presentation_modes : available_presentation_modes)
        {
            if (presentation_modes == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return VK_PRESENT_MODE_MAILBOX_KHR;
                present_mode_mailbox_found = true;
            }

            if (presentation_modes == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            {
                present_mode_fifo_relaxed_found = true;
            }

            if (present_mode_mailbox_found)
            {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }

        if (present_mode_fifo_relaxed_found)
        {
            return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        }

        /* Fall back case, since all gpu's will support this feature. (Guaranteed) */
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    VkExtent2D Renderer::select_swapchain_extent(const VkSurfaceCapabilitiesKHR& surface_capabilites)
    {
        /* If the width or height of extent is not sent to UINT32_MAX it means we have to manually set it. */

        VkExtent2D swapchain_extent;

        if (surface_capabilites.currentExtent.width == UINT32_MAX || surface_capabilites.currentExtent.height == UINT32_MAX)
        {
            swapchain_extent.width = std::clamp(swapchain_extent.width, surface_capabilites.minImageExtent.width, surface_capabilites.maxImageExtent.width);
            swapchain_extent.height = std::clamp(swapchain_extent.height, surface_capabilites.minImageExtent.height, surface_capabilites.maxImageExtent.height);
            return swapchain_extent;
        }
        else
        {
            /* Surface Extent has already been set for us. */
            return surface_capabilites.currentExtent;
        }
    }

    /* Helper functions for the graphics pipeline. */
    std::pair<VkShaderModule, VkShaderModule> Renderer::create_shader_modules(const char *vertex_shader_file_path, const char *fragment_shader_file_path)
    {
        std::vector<char> vertex_shader_contents = read_binary_file_from_path(vertex_shader_file_path);
        std::vector<char> fragment_shader_contents = read_binary_file_from_path(fragment_shader_file_path);

        VkShaderModule vertex_shader_module;
        VkShaderModule fragment_shader_module;

        /* One catch is that size of byte code is in bytes, but bytecode pointer (pCode) is in uint32_t. so, reinterpret cast it used.
         * There is no need to manually ensure data is satisfying the alignment requirement as std::vector takes care of this. */
        VkShaderModuleCreateInfo vertex_shader_module_create_info = {};
        vertex_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertex_shader_module_create_info.pNext = nullptr;
        vertex_shader_module_create_info.flags = 0;
        vertex_shader_module_create_info.codeSize = vertex_shader_contents.size();
        vertex_shader_module_create_info.pCode = reinterpret_cast<const uint32_t *>(vertex_shader_contents.data());

        VkResult shader_module_creation_result = vkCreateShaderModule(m_device, &vertex_shader_module_create_info, nullptr, &vertex_shader_module);

        if (shader_module_creation_result != VK_SUCCESS)
        {
            VK_CHECK(shader_module_creation_result);
            debug::error("Could not create vertex shader module.");
        }

        VkShaderModuleCreateInfo fragment_shader_module_create_info = {};
        fragment_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragment_shader_module_create_info.pNext = nullptr;
        fragment_shader_module_create_info.flags = 0;
        fragment_shader_module_create_info.codeSize = fragment_shader_contents.size();
        fragment_shader_module_create_info.pCode = reinterpret_cast<const uint32_t *>(fragment_shader_contents.data());

        shader_module_creation_result = vkCreateShaderModule(m_device, &fragment_shader_module_create_info, nullptr, &fragment_shader_module);

        if (shader_module_creation_result != VK_SUCCESS)
        {
            VK_CHECK(shader_module_creation_result);
            debug::error("Could not create vertex shader module.");
        }

        return std::pair(vertex_shader_module, fragment_shader_module);
    }


    void Renderer::destroy_debug_utils_messenger_ext
    (
        VkInstance instance,
        VkDebugUtilsMessengerEXT messenger,
        const VkAllocationCallbacks *allocator
    )
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func == nullptr)
        {
            debug::error("Could not load vkDestroyDebugUtilsMessengerEXT");
        }

        return func(instance, messenger, allocator);
    }
}