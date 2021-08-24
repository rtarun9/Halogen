#include "../../include/core/graphics/renderer.h"

//For Vulkan memory allocator
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace halogen
{
	Renderer::Renderer(Input &input): m_input(input)
	{
	}

    Renderer::~Renderer()
    {
        clean_up();
    }

    void Renderer::initialize_renderer(const Window& window)
    {
        initialize_vulkan(window);
		debug::log("Initialized renderer");

		load_mesh();

		initialize_descriptors();

        initialize_pipelines();

        initialize_scene();
    }

    void Renderer::render()
    {
	//	vkDeviceWaitIdle(m_device);

      	time::Clock::instance().m_frame_start = std::chrono::high_resolution_clock::now();

        //Block CPU until previous GPU command has not executed successfully.
        vk_check(vkWaitForFences(m_device, 1, &get_current_frame().m_render_fence, VK_TRUE, time::One_Second), "Failed waiting for fence. Timeout : 1 second");
        vk_check(vkResetFences(m_device, 1, &get_current_frame().m_render_fence));

        uint32_t swapchain_image_index;

        //Wait on the semaphore since you dont want to prepare next frame until GPU execution is done   .
        vk_check(vkAcquireNextImageKHR(m_device, m_swapchain_wrapper.m_swapchain, time::One_Second, get_current_frame().m_present_semaphore, nullptr, &swapchain_image_index));

        //Begin rendering commands

        vk_check(vkResetCommandBuffer(get_current_frame().m_command_buffer, 0));

        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.pNext = nullptr;
        command_buffer_begin_info.pInheritanceInfo = nullptr;

        //Let vulkan know this will command buffer will only be executed / submitted once. (It is being re - recorded every  frame).
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vk_check(vkBeginCommandBuffer(get_current_frame().m_command_buffer, &command_buffer_begin_info), "Failed to begin recording into command buffer.");

        VkClearValue clear_value;
        clear_value.color = {0.0f, 0.0f, 0.0f};

        VkClearValue depth_clear;
        depth_clear.depthStencil.depth = 1.0f;

        std::array<VkClearValue, 2> clear_values =
		{
			clear_value,
			depth_clear
		};

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;

        render_pass_begin_info.renderPass = m_render_pass;
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.renderArea.extent = m_extent;

        //Render into the target framebuffer (target framebuffer index : index of swapchain that has been freshly acquired).
        render_pass_begin_info.framebuffer = m_framebuffers[swapchain_image_index];

        render_pass_begin_info.clearValueCount = clear_values.size();
        render_pass_begin_info.pClearValues = clear_values.data();

        //not calling vk_check for begin render pass since it return void, and its guaranteed to work.
        //Bind the framebuffer, clear the image, and set image layout to that specified when creation of renderpass happened.
        vkCmdBeginRenderPass(get_current_frame().m_command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        m_main_scene.m_camera.update_camera(m_input);

        m_main_scene.render_objects(get_current_frame().m_command_buffer, get_current_frame(), m_vma_allocator);

        //Transitions the image layout so that it can be presented, as rendering is done.
        vkCmdEndRenderPass(get_current_frame().m_command_buffer);
        vk_check(vkEndCommandBuffer(get_current_frame().m_command_buffer));

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
        submit_info.pWaitSemaphores = &get_current_frame().m_present_semaphore;

        //Signal to the render semaphore about  rendering status.
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &get_current_frame().m_render_semaphore;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &get_current_frame().m_command_buffer;

        //Block CPU until graphics commands have finished their execution.
        vk_check(vkQueueSubmit(m_graphics_queue, 1, &submit_info, get_current_frame().m_render_fence), "Failed to submit to queue");

        //Present to screen after commands are submitted
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;

        present_info.pSwapchains = &m_swapchain_wrapper.m_swapchain;
        present_info.swapchainCount = 1;

        //Wait on render semaphore (presentation is blocked off until rendering is complete)
        present_info.pWaitSemaphores = &get_current_frame().m_render_semaphore;
        present_info.waitSemaphoreCount = 1;

        present_info.pImageIndices = &swapchain_image_index;

        vk_check(vkQueuePresentKHR(m_presentation_queue_extension, &present_info));

        time::Clock::instance().m_frame_end = std::chrono::high_resolution_clock::now();
        time::Clock::instance().m_frame_time = std::chrono::duration_cast<std::chrono::duration<double>>
		(
                time::Clock::instance().m_frame_end - time::Clock::instance().m_frame_start
		);

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
                .request_validation_layers(true)
                .require_api_version(1, 1, 0)
                .use_default_debug_messenger()
                .build();

        if (!instance)
        {
            debug::error("Failed to create vulkan instance.", "Error : ", instance.error().message());
        }

        vkb::Instance vkb_instance = instance.value();
        m_instance = vkb_instance.instance;

        m_debug_messenger = vkb_instance.debug_messenger;

        Platform::create_window_surface(window.get_window(), m_instance, m_window_surface);

        //Pass in the window surface so that physical device selector will choose GPU that can actually render onto the screen.
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
        m_presentation_queue_extension = vkb_device.get_queue(vkb::QueueType::present).value();
        m_queue_family_indices.m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        //Initializing VMA here as well
        VmaAllocatorCreateInfo vma_allocator_create_info = {};
        vma_allocator_create_info.instance = m_instance;
        vma_allocator_create_info.device = m_device;
        vma_allocator_create_info.physicalDevice = m_physical_device;

        vk_check(vmaCreateAllocator(&vma_allocator_create_info, &m_vma_allocator), "Failed to initialize VMA");
    }

    //Uses vk_boostrap to create swapchain.
    void Renderer::create_swapchain()
    {
        vkb::SwapchainBuilder swapchain_builder {m_physical_device, m_device, m_window_surface};

        //Gpu never idles here (since mail box is used). If power consumption is a issue, use V-Sync (with FIFO present modes, and plus its guarenteed to exist).
        vkb::Swapchain vkb_swapchain = swapchain_builder
                    .use_default_format_selection()
                    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR)
                    .set_desired_extent(m_extent.width, m_extent.height)
                    .build()
                    .value();

        m_swapchain_wrapper.m_swapchain = vkb_swapchain.swapchain;

        //VkImage is the actual image that is used as texture to render to.
        //VkImageView is a wrapper for the image (allows to do things such as swap colors, etc).
        m_swapchain_wrapper.m_swapchain_images = vkb_swapchain.get_images().value();
        m_swapchain_wrapper.m_swapchain_image_views = vkb_swapchain.get_image_views().value();
        m_swapchain_wrapper.m_swapchain_image_format = vkb_swapchain.image_format;

        //Push into deletion queue
        m_deletion_queue.push_function([=]()
        {
            vkDestroySwapchainKHR(m_device, m_swapchain_wrapper.m_swapchain, nullptr);
        });

        //For depth image
        VkExtent3D depth_image_extent =
		{
        	m_extent.width,
        	m_extent.height,
        	1
		};

        //Format : 32 bit floats.
		m_depth_image_wrapper.m_depth_format = VK_FORMAT_D32_SFLOAT;

        VkImageCreateInfo depth_image_create_info = vkinit::image_objects::create_image_create_info(m_depth_image_wrapper.m_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depth_image_extent);

        //For depth image, allocate from GPU's local memory (Vram)
        VmaAllocationCreateInfo depth_image_allocation_info = {};
    	depth_image_allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    	depth_image_allocation_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    	vmaCreateImage(m_vma_allocator, &depth_image_create_info, &depth_image_allocation_info, &m_depth_image_wrapper.m_depth_image.m_image, &m_depth_image_wrapper.m_depth_image.m_allocation, nullptr);

    	VkImageViewCreateInfo depth_image_view_create_info = vkinit::image_objects::create_image_view_create_info(m_depth_image_wrapper.m_depth_format, m_depth_image_wrapper.m_depth_image.m_image, VK_IMAGE_ASPECT_DEPTH_BIT);

    	vk_check(vkCreateImageView(m_device, &depth_image_view_create_info, nullptr, &m_depth_image_wrapper.m_depth_image_view), "Failed to create depth image view.");

    	//Push image and image view to queue
    	m_deletion_queue.push_function([=]()
    	{
			vkDestroyImageView(m_device, m_depth_image_wrapper.m_depth_image_view, nullptr);
			vmaDestroyImage(m_vma_allocator, m_depth_image_wrapper.m_depth_image.m_image, m_depth_image_wrapper.m_depth_image.m_allocation);
		});
    }

    //Create command buffers and command objects.
    void Renderer::create_command_objects()
    {
		//CommandPool : allocator for command buffers.
		//flag used here is to specify that command buffers will be reset using vkResetCommandBuffer. (Its state needs to be reset from pending to initial.)
		VkCommandPoolCreateInfo command_pool_create_info = vkinit::command_objects::create_command_pool_create_info(m_queue_family_indices.get_graphics_queue_family(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		for (int i = 0; i < m_frames.size(); i++)
		{
			vk_check(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_frames[i].m_command_pool), "Failed to create comamnd pool");

			//primary level command buffer, are sent into the Vkqueue's (execution port of GPU).
			//secondary level command buffers, are sub buffers to the to the primary command buffer.
			VkCommandBufferAllocateInfo command_buffer_allocate_info = vkinit::command_objects::create_command_buffer_allocate_info(m_frames[i].m_command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			vk_check(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_frames[i].m_command_buffer), "Failed to allocate command buffer");

			//Delete the command pools (command buffers will be deleted with this as well)
			m_deletion_queue.push_function([=]()
			{
				vkDestroyCommandPool(m_device, m_frames[i].m_command_pool, nullptr);
			});
		}
    }

    //Creating of renderpass (blue print of output framebuffer)
    void Renderer::create_render_pass()
    {
        //Point of renderpass : store details of the image we are writing into (renderpass writes into the framebuffer, which links to the actual image we are writing into).
        //Also does image layout transition (since images are generally stored in someother format by GPU for optimization).

        //Note : Image layouts :
        //UNDEFINED -> Renderpass begins -> Subpass 0 begins -> Image layout set to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (optimal for writing into image) ->
        // Subpass 0 renders -> Subpass 0 ends -> Renderpass ends -> Image layout set to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.

        //Desciption of image we will be writing into, using the renderpass.
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = m_swapchain_wrapper.m_swapchain_image_format;

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

        //Depth attachment
        VkAttachmentDescription depth_attachment = {};
        depth_attachment.flags = 0;
        depth_attachment.format = m_depth_image_wrapper.m_depth_format;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        //Dont care about stencil
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        //Initial and final layout
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference color_attachment_reference = {};

        //Reference into the render_pass's pAttachment array. layout is set to color optimal since it will be optimal for writing into the color attachment.
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_reference = {};
        depth_attachment_reference.attachment = 1;
        depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentDescription , 2> attachments =
		{
        	color_attachment,
        	depth_attachment
		};

        //Only setting up 1 subpass (which is the minimal). Wont be used much in the engine.
        VkSubpassDescription subpass_description = {};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_attachment_reference;
        subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = attachments.size();
        render_pass_create_info.pAttachments = attachments.data();
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass_description;
		render_pass_create_info.dependencyCount = 0;
		render_pass_create_info.pDependencies = nullptr;

        vk_check(vkCreateRenderPass(m_device, &render_pass_create_info, nullptr, &m_render_pass), "Failed to create render pass.");

        //Add renderpass deletion to the deletion queue
        m_deletion_queue.push_function([=] ()
        {
            vkDestroyRenderPass(m_device, m_render_pass, nullptr);
        });
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

        const uint32_t swapchain_image_count = m_swapchain_wrapper.m_swapchain_images.size();
        m_framebuffers.resize(swapchain_image_count);

        for (int i = 0; i < swapchain_image_count; i++)
        {
        	//Connect depth image view and each of the frame buffers.
        	VkImageView attachments[2] =
			{
        		m_swapchain_wrapper.m_swapchain_image_views[i],
				m_depth_image_wrapper.m_depth_image_view
			};

			framebuffer_create_info.attachmentCount = 2;
			framebuffer_create_info.pAttachments = attachments;

            vk_check(vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffers[i]), "Failed to create framebuffers");

            //Add framebuffer deletion to deletion queue
            m_deletion_queue.push_function([=]()
            {
                //Note : Adding image view deletion code here as well since both have to be deleted together anyway
                vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
                vkDestroyImageView(m_device, m_swapchain_wrapper.m_swapchain_image_views[i], nullptr);
            });
        }

    }

    //Fence and Semaphore creation.
    void Renderer::create_sync_objects()
    {
		for (int i = 0; i < m_frames.size(); i++)
		{
			//Create Fence for GPU - CPU communication.
			VkFenceCreateInfo fence_create_info = {};
			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_create_info.pNext = nullptr;

			//Create fence in the signalled state, so that when we wait on the flag for the first iteration of render loop, it immediately passes [edge case].
			fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			vk_check(vkCreateFence(m_device, &fence_create_info, nullptr, &m_frames[i].m_render_fence), "Failed to create fence.");

			//Implement deletion queue for fences
			m_deletion_queue.push_function([=]()
			{
				vkDestroyFence(m_device,  m_frames[i].m_render_fence, nullptr);
			});

			//Create semaphores for GPU - GPU communication.
			VkSemaphoreCreateInfo semaphore_create_info = {};
			semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphore_create_info.pNext = nullptr;
			semaphore_create_info.flags = 0;

			vk_check(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_frames[i].m_present_semaphore), "Failed to create presentation semaphore");
			vk_check(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_frames[i].m_render_semaphore), "Failed to create render semaphore");

			//Implement deletion queue for semaphores.
			m_deletion_queue.push_function([=]()
			{
				vkDestroySemaphore(m_device, m_frames[i].m_present_semaphore, nullptr);
				vkDestroySemaphore(m_device, m_frames[i].m_render_semaphore, nullptr);
			});
		}
    }

    void Renderer::initialize_descriptors()
	{
		//Create descriptor pool for 10 uniform buffer (10 is just a high number, nothing special about it)
		VkDescriptorPoolSize descriptor_pools = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10};

		VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};

		//Need to remove hard coded values here
		descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_create_info.flags = 0;
		descriptor_pool_create_info.maxSets = 10;
		descriptor_pool_create_info.poolSizeCount = 1;
		descriptor_pool_create_info.pPoolSizes = &descriptor_pools;

		vk_check(vkCreateDescriptorPool(m_device, &descriptor_pool_create_info, nullptr, &m_descriptor_pool), "Failed to create descriptor pool.");

		//Information about the binding
		VkDescriptorSetLayoutBinding camera_buffer_binding = {};
		camera_buffer_binding.binding = 0;
		camera_buffer_binding.descriptorCount = 1;
		camera_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		camera_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {};
		descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pNext = nullptr;

		descriptor_set_layout_create_info.bindingCount = 1;
		descriptor_set_layout_create_info.flags = 0;
		descriptor_set_layout_create_info.pBindings = &camera_buffer_binding;

		vk_check(vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &m_global_descriptor_set_layout), "Failed to create global descriptor set layout");
		for (int i = 0; i < m_frames.size(); i++)
		{
			m_frames[i].m_camera_buffer = vkinit::buffers::create_buffer(sizeof(wrapper::CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, m_vma_allocator);

			//Allocate one descriptor per frame
			VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
			descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptor_set_allocate_info.pNext = nullptr;
			descriptor_set_allocate_info.descriptorPool = m_descriptor_pool;
			descriptor_set_allocate_info.descriptorSetCount = 1;
			descriptor_set_allocate_info.pSetLayouts = &m_global_descriptor_set_layout;

			vk_check(vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &m_frames[i].m_global_descriptor), "Failed to allocate descriptor set for a frame");

			//Buffer we want ot point at in the descriptor
			VkDescriptorBufferInfo descriptor_buffer_info = {};
			descriptor_buffer_info.buffer = m_frames[i].m_camera_buffer.m_buffer;
			descriptor_buffer_info.offset = 0;
			descriptor_buffer_info.range = sizeof(wrapper::CameraData);

			VkWriteDescriptorSet set_write = {};
			set_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			set_write.pNext = nullptr;

			set_write.dstBinding = 0;
			set_write.descriptorCount = 1;
			set_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			set_write.pBufferInfo = &descriptor_buffer_info;
			set_write.dstSet = m_frames[i].m_global_descriptor;

			vkUpdateDescriptorSets(m_device, 1, &set_write, 0, nullptr);
		}
	}
    //Pipeline related stuff, move later
    void Renderer::initialize_pipelines()
    {
        //Stuff for shader stages (both fragment and vertex shader).
        VkShaderModule triangle_vertex_shader_module;
        VkShaderModule triangle_fragment_shader_module;

        pipeline_utils::create_shader_module(m_device, "vertex_buffer_vertex_shader.spv", &triangle_vertex_shader_module);
        pipeline_utils::create_shader_module(m_device, "vertex_buffer_fragment_shader.spv", &triangle_fragment_shader_module);

        VertexInputDescription vertex_input_description = Vertex::get_vertex_input_description();

        //How to read vertex buffers.
        m_pipeline_config.m_vertex_input_stage_info = vkinit::pipeline_objects::create_vertex_input_state_create_info();
        m_pipeline_config.m_vertex_input_stage_info.pVertexAttributeDescriptions = vertex_input_description.m_attribute_descriptions.data();
        m_pipeline_config.m_vertex_input_stage_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_description.m_attribute_descriptions.size());
        m_pipeline_config.m_vertex_input_stage_info.pVertexBindingDescriptions = vertex_input_description.m_binding_descriptions.data();
        m_pipeline_config.m_vertex_input_stage_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_description.m_binding_descriptions.size());

        //What do you want to draw (topology).
        m_pipeline_config.m_input_assembly_state_info = vkinit::pipeline_objects::create_input_assembly_state_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        //Viewport tell's the pipeline how to transform the gl_Position of the vertex (basically, determine which part of the screen you want to render to. viewport does the transformation from a (-1, 1) range to a (0, width), (0, height) range).
        m_pipeline_config.m_viewport.x = 0.0f;
        m_pipeline_config.m_viewport.y = 0.0f;
        m_pipeline_config.m_viewport.width = static_cast<float>(m_extent.width);
        m_pipeline_config.m_viewport.height = static_cast<float>(m_extent.height);

        //Transformation details for the Z (depth) part of the vertex.
        m_pipeline_config.m_viewport.minDepth = 0.0f;
        m_pipeline_config.m_viewport.maxDepth = 1.0f;

        //Specify the area where rendering is allowed. Anything outside the scissor rectangle will be cut(well, thats why its called scissor).
        m_pipeline_config.m_scissor_rectangle.offset = {0, 0};
        m_pipeline_config.m_scissor_rectangle.extent = m_extent;

        //Draw filled rectangles for now..
        m_pipeline_config.m_rasterization_state_create_info = vkinit::pipeline_objects::create_rasterization_state_create_info(VK_POLYGON_MODE_FILL);

        m_pipeline_config.m_multisample_state_create_info = vkinit::pipeline_objects::create_multisample_state_create_info();

        //No blending, but write into RGBA
        m_pipeline_config.m_color_blend_attachment = vkinit::pipeline_objects::create_color_blend_attachment_state();

        //For Z buffer
        m_pipeline_config.m_depth_stencil_state_create_info = vkinit::pipeline_objects::create_depth_stencil_state_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
        //Create and use the pipeline layout.
        VkPipelineLayoutCreateInfo pipeline_layout_create_info =  vkinit::pipeline_objects::create_pipeline_layout_create_info();

        //Push constants setup
        VkPushConstantRange push_constant_range;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(MeshPushConstants);
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;
        pipeline_layout_create_info.pushConstantRangeCount = 1;

        //Set up global descriptor set layout
        pipeline_layout_create_info.setLayoutCount = 1;
        pipeline_layout_create_info.pSetLayouts = &m_global_descriptor_set_layout;

        vk_check(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_config.m_layout), "Failed to create pipeline layout");

        //Bind shader stages for the rgb triangle
        m_pipeline_config.m_shader_stages.push_back(vkinit::pipeline_objects::create_pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangle_vertex_shader_module));
        m_pipeline_config.m_shader_stages.push_back(vkinit::pipeline_objects::create_pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangle_fragment_shader_module));

        //Build pipeline for rgb triangle
        m_primary_pipeline = m_pipeline_config.build_pipeline(m_device, m_render_pass);

        //Delete shader modules (no need for this to be done in the deletion queue, just plain old deletion).
        vkDestroyShaderModule(m_device, triangle_vertex_shader_module, nullptr);
        vkDestroyShaderModule(m_device, triangle_fragment_shader_module, nullptr);

        //Deletion queue for pipeline related objects
        m_deletion_queue.push_function([=]()
        {
            vkDestroyPipeline(m_device, m_primary_pipeline, nullptr);

            vkDestroyPipelineLayout(m_device, m_pipeline_config.m_layout, nullptr);
        });

        m_main_scene.create_material(m_primary_pipeline, m_pipeline_config.m_layout, "default");
    }

    //Mesh related functions
    void Renderer::load_mesh()
    {
        m_triangle_mesh.m_vertices.resize(3);

        m_triangle_mesh.m_vertices[0].m_position = glm::vec3(0.0f, -0.5f, 0.0f);
        m_triangle_mesh.m_vertices[1].m_position = glm::vec3(-0.5f, 0.5f, 0.0f);
        m_triangle_mesh.m_vertices[2].m_position = glm::vec3(0.5f, 0.5f, 0.0f);

        m_triangle_mesh.m_vertices[0].m_color = glm::vec3(1.0f, 1.0f, 0.0f);
        m_triangle_mesh.m_vertices[1].m_color = glm::vec3(0.5f, 1.0f, 0.0f);
        m_triangle_mesh.m_vertices[2].m_color = glm::vec3(1.0f, 0.0f, 1.0f);

        //Dont care about normals for now, so setting everything to zero by default.
        m_triangle_mesh.m_vertices[0].m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
        m_triangle_mesh.m_vertices[1].m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
        m_triangle_mesh.m_vertices[2].m_normal = glm::vec3(0.0f, 0.0f, 0.0f);

        auto file_path = std::string("../assets/monkey_smooth.obj");
        m_monkey_mesh.load_obj_from_file(file_path);

        upload_mesh(m_triangle_mesh);
		upload_mesh(m_monkey_mesh);

		m_main_scene.m_meshes["monkey"] = m_monkey_mesh;
		m_main_scene.m_meshes["triangle"] = m_triangle_mesh;
    }

    void Renderer::upload_mesh(Mesh &mesh)
    {
        //Allocate the vertex buffer
        VkBufferCreateInfo vertex_buffer_create_info = {};
        vertex_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertex_buffer_create_info.size = mesh.m_vertices.size() * sizeof(Vertex);

        //Tell vulkan this buffer will be used as a vertex buffer
        vertex_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        //[NOTE] : Host is the CPU, Device is the GPU. VMA internally uses HOST_VISIBLE_BIT and HOST_COHERENT_BIT to make sure both memory regions are coherent and in sync.

        //This data should we writable by CPU, but should be read from GPU and the allocation is directly is placed directly in GPU-accessible memory.
        VmaAllocationCreateInfo vma_allocation_create_info = {};
        vma_allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vk_check(vmaCreateBuffer(m_vma_allocator, &vertex_buffer_create_info, &vma_allocation_create_info, &mesh.m_vertex_buffer.m_buffer, &mesh.m_vertex_buffer.m_allocation, nullptr), "Failed to create and allocate vertex buffer");

        //Add to deletion queue
        m_deletion_queue.push_function([=]()
        {
            vmaDestroyBuffer(m_vma_allocator, mesh.m_vertex_buffer.m_buffer, mesh.m_vertex_buffer.m_allocation);
        });

        //Memory management
        //Copy your mesh.m_vertices data into a region were GPU can read from (this is some memory on the cpu accessable by the GPU)
        void *data;

        //Create regions of host memory (CPU) mapped to the Device (GPU memory)
        vmaMapMemory(m_vma_allocator, mesh.m_vertex_buffer.m_allocation, &data);

        //Copies data into host mapped memory, and since both host and device are coherent, should update Device memory too.
        memcpy(data, mesh.m_vertices.data(), static_cast<uint32_t>(mesh.m_vertices.size()) * sizeof(Vertex));

        vmaUnmapMemory(m_vma_allocator, mesh.m_vertex_buffer.m_allocation);
    }

    void Renderer::initialize_scene()
	{
		GameObject monkey;
		monkey.m_mesh = m_main_scene.get_mesh("monkey");
		monkey.m_material = m_main_scene.get_material("default");
		monkey.m_transformation_mat = glm::mat4{ 1.0f };

		m_main_scene.m_game_objects.push_back(monkey);

		for (int x = -20; x <= 20; x++)
		{
			for (int y = -20; y <= 20; y++)
			{

				GameObject tri;
				tri.m_mesh = m_main_scene.get_mesh("triangle");
				tri.m_material = m_main_scene.get_material("default");

				glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
				glm::mat4 rotation = glm::rotate(glm::mat4{1.0f}, glm::radians((float)tri.get_id() / 3), glm::vec3(1.0f, 1.0f, 1.0f));
				glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
				tri.m_transformation_mat = translation * scale * rotation;

				m_main_scene.m_game_objects.push_back(tri);
			}
		}

		m_main_scene.m_scene_initialized = true;
		
		debug::log("Initialized main scene.");
	}

	wrapper::FrameData& Renderer::get_current_frame()
	{
		return m_frames[m_frame_number % configuration::MAX_FRAMES_IN_FLIGHT];
	}

    void Renderer::clean_up()
    {
		vkDeviceWaitIdle(m_device);

        //Make sure GPU has stopped doing all its operations before deletion
        vkWaitForFences(m_device, 1, &get_current_frame().m_render_fence, true, time::One_Second);

        m_deletion_queue.flush();

        vmaDestroyAllocator(m_vma_allocator);

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_window_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);

        vkDestroyInstance(m_instance, nullptr);
    }
}