#pragma once

#include <string>

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
    constexpr int DEFAULT_WINDOW_WIDTH = 1080;
    constexpr int DEFAULT_WINDOW_HEIGHT  = 720;
    constexpr const char *DEFAULT_WINDOW_TITLE = "core engine";

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;
}

//[NOTE] : Remove the configuration namespace when the Config struct is complete
struct Config
{
	PlatformBackend m_platform_backend;
	int m_window_width;
	int m_window_height;
	std::string m_window_name;
};
