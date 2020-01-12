#include <stdexcept>

#include "sdlplatform.h"

int SDL_main(int argc, char* argv[])
{
    SDLPlatform platform;
    try
    {
        platform.init(argc, argv);
        platform.main();
        return 0;
    }
    catch (std::runtime_error error)
    {
        SDL_ShowSimpleMessageBox(0, "Error", error.what(), nullptr);
        return 1;
    }
}
