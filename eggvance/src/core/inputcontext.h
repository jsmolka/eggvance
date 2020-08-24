#pragma once

#include "base/int.h"
#include "base/sdl.h"

class InputContext
{
public:
    friend class Context;

    uint state() const;

    void processDeviceEvent(const SDL_ControllerDeviceEvent& event);

private:
    enum
    {
        kA      = 0,
        kB      = 1,
        kSelect = 2,
        kStart  = 3,
        kRight  = 4,
        kLeft   = 5,
        kUp     = 6,
        kDown   = 7,
        kR      = 8,
        kL      = 9
    };

    void init();
    void deinit();

    uint keyboardState() const;
    uint controllerState() const;

    SDL_GameController* controller = nullptr;
};
