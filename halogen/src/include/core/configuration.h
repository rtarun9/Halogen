#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* Multiple platform backends can be used in future. */

enum class PlatformBackend
{
    SDL2 = 1,
    Win32 = 2,
    GLFW = 3,
};


namespace configuration
{
    /* Currently only SDL2 is available as a platform backend. */
    constexpr static PlatformBackend PLATFORM_BACKEND = PlatformBackend::SDL2;

    constexpr static bool DEBUG = true;
    constexpr static int DEFAULT_WINDOW_WIDTH = 720;
    constexpr static int DEFAULT_WINDOW_HEIGHT  = 680;
    constexpr static const char *DEFAULT_WINDOW_TITLE = "halogen engine";

    constexpr static const char *VALIDATION_LAYERS[1] =
    {
        "VK_LAYER_KHRONOS_validation"
    };
}
#endif