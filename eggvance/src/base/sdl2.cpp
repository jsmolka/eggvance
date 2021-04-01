#include "sdl2.h"

#include <shell/format.h>

void showMessageBox(const std::string& title, const std::string& message)
{
    SDL_ShowSimpleMessageBox(0, title.c_str(), message.c_str(), NULL);
}
