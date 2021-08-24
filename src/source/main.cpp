#include "../include/halogen.h"

#include <cstdlib>
#include <memory>
#include <iostream>

int main (int argc, char *argv[])
{
	//[NOTE] : Config is WIP for now
	//Add engine config over here rather than in the configuration file
	Config config =
	{
		.m_platform_backend = PlatformBackend::SDL2,
		.m_window_width = 1080,
		.m_window_height = 720,
		.m_window_name = "Halogen Engine"
	};

    std::unique_ptr<halogen::Engine> engine;
    engine = std::make_unique<halogen::Engine>(config);

    try
    {
        engine->start();
    }
    catch (std::exception &exception)
    {
        std::cout << "[EXCEPTION CAUGHT] : " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}