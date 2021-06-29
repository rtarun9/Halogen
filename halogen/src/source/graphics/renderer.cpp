#include "../../include/core/graphics/renderer.h"

namespace halogen
{
    /* Core Renderer functions. */
    Renderer::Renderer(std::shared_ptr<Window> &window_reference) : m_window_reference(window_reference)
    {
        m_vulkan_instance = VK_NULL_HANDLE;

        m_physical_device = VK_NULL_HANDLE;
        m_device = VK_NULL_HANDLE;

        m_window_surface = VK_NULL_HANDLE;

        m_graphics_queue = VK_NULL_HANDLE;
        m_presentation_queue = VK_NULL_HANDLE;
    }

    void Renderer::initialize_renderer()
    {
        initialize_vulkan();
    }

    void Renderer::render()
    {

    }

    void Renderer::cleanup()
    {
        if (configuration::VALIDATION_LAYERS)
        {
            destroy_debug_utils_messenger_ext(m_vulkan_instance, m_debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(m_vulkan_instance, m_window_surface, nullptr);
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
        VK_CHECK(instance_creation_result);
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
        VK_CHECK(debug_utils_messenger_creation_result);
    }

    /* Included before physical device selection as window surface may influence the physical device selection process. */
    void Renderer::create_window_surface()
    {
        VkResult window_surface_creation_result = create_platform_specific_window_surface();
        if (window_surface_creation_result != VK_SUCCESS)
        {
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
            device_create_info.ppEnabledLayerNames = configuration::VALIDATION_LAYERS;
            device_create_info.enabledLayerCount = configuration::VALIDATION_LAYER_COUNT;
        }
        else
        {
            /* Not really necessary, since newer vulkan api's dont distinguish between instance / device specific validation layers. */
            device_create_info.ppEnabledLayerNames = nullptr;
            device_create_info.enabledExtensionCount = 0;
        }

        device_create_info.enabledExtensionCount = 0;
        device_create_info.ppEnabledExtensionNames = nullptr;

        VkResult device_creation_result = vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device);
        VK_CHECK(device_creation_result);

        vkGetDeviceQueue(m_device, queue_family_indices.m_graphics_queue_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, queue_family_indices.m_presentation_queue_family.value(), 0, &m_presentation_queue);
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
                      << callback_data->pObjects << '\n';
        }
        return VK_FALSE;
    }

    /* Vulkan helper functions. */
    void Renderer::check_extension_support(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count)
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

        check_extension_support(instance_extension_names, instance_extension_count);
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

        return queue_family_index.is_queue_family_complete();
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