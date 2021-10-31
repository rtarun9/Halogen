#include "../include/engine.h"

int main(int argc, char** argv)
{
	halo::Engine engine;
	
	engine.initialize();
	engine.run();
	engine.clean();

	return 0;
}