#pragma once

#include <deque>
#include <functional>

namespace halogen
{
	//Holds all queue families and queue extensions.
	struct  QueueFamilyIndices
	{
		uint32_t get_graphics_queue_family()
		{
			return m_graphics_queue_family.value();
		}

		uint32_t get_transfer_queue_family()
		{
			return m_transfer_queue_family.value();
		}

		uint32_t get_presentation_queue_family_extension()
		{
			return m_presentation_queue_family_extension.value();
		}

		bool is_queue_family_complete()
		{
			return m_graphics_queue_family.has_value() && m_transfer_queue_family.has_value();
		}

		std::optional<uint32_t> m_transfer_queue_family;
		std::optional<uint32_t> m_graphics_queue_family;

		std::optional<uint32_t> m_presentation_queue_family_extension;
	};

	//Basic struct for holding allocated buffer and its data
	struct AllocatedBuffer
	{
		//Vulkan GPU side buffer.
		VkBuffer m_buffer;

		//If vma wasn't used, this would be there
		//VkDeviceMemory m_vertex_buffer_memory;

		//Hold state VMA uses (size, memory buffer was allocated from).
		VmaAllocation m_allocation;
	};

	//Destruction queue : easy automated way of calling vkDestroyXXX commands in the right order.
    struct DeletionQueue
    {
    public:
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function)
        {
            deletors.push_back(function);
        }

        //This will call the lambda functions in the same order that they were queued in.
        //Not a very efficient way, but gets the job done.
        void flush()
        {
            //Using rbegin and rend here since we want to iterate over the double ended queue in reverse, because
            //Usually objects in vulkan are deleted in the reverse order they were implemented.
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                (*it)();
            }

            deletors.clear();
        }
    };
}