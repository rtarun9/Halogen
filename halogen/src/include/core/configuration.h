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
    constexpr PlatformBackend PLATFORM_BACKEND = PlatformBackend::SDL2;

    constexpr int DEFAULT_WINDOW_WIDTH = 720;
    constexpr int DEFAULT_WINDOW_HEIGHT  = 680;
    constexpr const char *DEFAULT_WINDOW_TITLE = "halogen engine";
}
#endif