#include "message.h"

#include <SDL2/SDL_messagebox.h>

void showMessage(const char* message)
{
    SDL_ShowSimpleMessageBox(0, "Error", message, nullptr);
}
