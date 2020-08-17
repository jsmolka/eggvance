#pragma once

#include "base/input.h"
#include "base/sdl2.h"

class InputContext
{
public:
    friend class Context;

    uint state() const;

    void processDeviceEvent(const SDL_ControllerDeviceEvent& event);

private:
    enum
    {
        kBitA      = 0,
        kBitB      = 1,
        kBitSelect = 2,
        kBitStart  = 3,
        kBitRight  = 4,
        kBitLeft   = 5,
        kBitUp     = 6,
        kBitDown   = 7,
        kBitR      = 8,
        kBitL      = 9
    };

    void init();
    void deinit();

    uint keyboardState() const;
    uint controllerState() const;

    SDL_GameController* controller = nullptr;
};
