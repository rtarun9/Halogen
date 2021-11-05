#include "../include/engine.h"

int main(int argc, char** argv)
{
	halo::Config config;
	config.m_window_width = 1080;
	config.m_window_height = 720;
	config.m_window_name = "halo";

	halo::Engine engine(config);
	
	engine.initialize();
	engine.run();
	engine.clean();

	return 0;
}