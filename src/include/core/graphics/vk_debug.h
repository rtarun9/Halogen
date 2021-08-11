#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace halogen
{
    inline void vk_check(VkResult err, std::string message = "")
    {
        if (err != VK_SUCCESS)
        {
        	std::cout << "[ERROR : ] " << message << std::endl;
        	throw std::runtime_error("Failed stuff");
        }
    }
}


