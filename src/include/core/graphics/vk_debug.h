#pragma once

#include "../../log.h"

#include <vulkan/vulkan.h>

namespace halogen
{
    inline void vk_check(VkResult err, const char *message = "")
    {
        if (err != VK_SUCCESS)
        {
            //debug::error automatically exits engine, no need to do it here explicitly.
            debug::error("Encountered vulkan error : ", message);
        }
    }
}


