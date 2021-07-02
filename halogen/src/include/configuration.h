#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* Multiple platform backends can be used in future. */

enum class PlatformBackend
{
    SDL2 = 1,
    Win32 = 2,
    GLFW = 3,
};

/* Contains the current engine configuration details. */

namespace configuration
{
    /* Currently only SDL2 is available as a platform backend. */
    constexpr static PlatformBackend PLATFORM_BACKEND = PlatformBackend::SDL2;

    constexpr static bool DEBUG = true;
    constexpr static int DEFAULT_WINDOW_WIDTH = 720;
    constexpr static int DEFAULT_WINDOW_HEIGHT  = 680;
    constexpr static const char *DEFAULT_WINDOW_TITLE = "halogen engine";

    constexpr static int VALIDATION_LAYER_COUNT = 1;

    constexpr static const char *VALIDATION_LAYERS[VALIDATION_LAYER_COUNT] =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    constexpr static int DEVICE_EXTENSION_COUNT = 1;
    constexpr static const char *DEVICE_EXTENSIONS[DEVICE_EXTENSION_COUNT] =
    {
        "VK_KHR_swapchain"
    };
}
#endif