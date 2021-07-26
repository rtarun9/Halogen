#include "../include/halogen.h"

#include <cstdlib>
#include <memory>

int main (int argc, char *argv[])
{
    std::unique_ptr<halogen::Engine> engine;
    engine = std::make_unique<halogen::Engine>();

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