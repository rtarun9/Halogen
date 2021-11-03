#pragma once

#include <vector>
#include <functional>

#include <vulkan/vulkan.h>

namespace halo
{
	struct DeletionList
	{
		std::vector<std::function<void()>> deletors;

		// significance of && here : lvalue is copied, while r value is moved.
		// mostly (almost always) the capture will be by value. Reason for this is that the objects will go outofscope in that particular function block, and we want to preserve it.
		// hence we do a copy by value.
		void push_function(std::function<void()>&& function)
		{
			deletors.push_back(function);
		}

		void clear_deletor_list()
		{
			for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
			{
				(*it)();
			}

			deletors.clear();
		}
	};
}