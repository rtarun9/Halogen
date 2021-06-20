#include "../include/halogen.h"

#include <stdexcept>
#include <cstdlib>

int main (int argc, char *argv[])
{
    halogen::Engine engine;

    try
    {
        engine.run();
    }
    catch (std::exception &exception)
    {
        std::cout << "[EXCEPTION CAUGHT] : " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}