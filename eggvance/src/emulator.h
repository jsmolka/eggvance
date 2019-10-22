#pragma once

#include <SDL2/SDL.h>

#include "args.h"

class Emulator
{
public:
    Emulator();
    void reset();

    bool init(const Args& args);
    
    void run();

private:
    void drawIcon();
    bool dropAwait();
    bool dropEvent(const SDL_DropEvent& event);
    void updateWindowTitle();

    void frame();

    std::string title;
};
