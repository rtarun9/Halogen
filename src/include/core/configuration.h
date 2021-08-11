#pragma once

//Multiple platform backends can be used in future.
enum class PlatformBackend
{
    SDL2 = 1,
    Win32 = 2,
    GLFW = 3,
};

//Contains the current engine configuration details.
namespace configuration
{
    //Currently only SDL2 is available as a platform backend.
    constexpr static PlatformBackend PLATFORM_BACKEND = PlatformBackend::SDL2;

    constexpr bool DEBUG = true;
    constexpr int DEFAULT_WINDOW_WIDTH = 720;
    constexpr int DEFAULT_WINDOW_HEIGHT  = 680;
    constexpr const char *DEFAULT_WINDOW_TITLE = "core engine";
}
