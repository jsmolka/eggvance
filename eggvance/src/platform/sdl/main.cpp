#include <stdexcept>

#include "sdlplatform.h"

#ifdef _MSC_VER
#define EGG_MAIN SDL_main
#else
#define EGG_MAIN main
#endif

int EGG_MAIN(int argc, char* argv[])
{
    SDLPlatform platform;
    try
    {
        platform.init(argc, argv);
        platform.main();
        return 0;
    }
    catch (const std::runtime_error& error)
    {
        SDL_ShowSimpleMessageBox(0, "Error", error.what(), nullptr);
        return 1;
    }
}
