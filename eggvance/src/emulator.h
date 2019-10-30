#pragma once

#include <string>
#include <SDL2/SDL.h>

#include "common/config.h"
#include "framelimiter.h"

class Emulator
{
public:
    Emulator();
    void reset();

    bool init(const std::string& rom);
    
    void run();

private:
    void drawIcon();
    bool dropAwait();

    void keyboardEvent(const SDL_KeyboardEvent& event);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event);
    void handleShortcut(Config::Shortcut shortcut);
    bool dropEvent(const SDL_DropEvent& event);
    void updateWindowTitle();

    void frame();

    std::string title;
    FrameLimiter limiter;
};
