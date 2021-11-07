#include "../include/engine.h"

int main(int argc, char** argv)
{
	halo::Config config;
	config.m_window_width = 1080;
	config.m_window_height = 720;
	config.m_window_name = "halo";

	// will put most code into a App class in the future, after engine's core features are setup and ready
	try
	{
		halo::Engine engine(config);
	}
	catch (vk::SystemError& err)
	{
		std::cout << "vk::SystemError: " << err.what() << std::endl;
		exit(-1);
	}
	catch (std::exception& err)
	{
		std::cout << "std::exception: " << err.what() << std::endl;
		exit(-1);
	}
	catch (...)
	{
		std::cout << "unknown error\n";
		exit(-1);
	}

	return 0;
}