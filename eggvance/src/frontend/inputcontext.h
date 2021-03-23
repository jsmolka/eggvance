#pragma once

#include "base/int.h"
#include "base/sdl2.h"

class InputContext
{
public:
    ~InputContext();

    void init();

    uint state() const;

    void processDeviceEvent(const SDL_ControllerDeviceEvent& event);

private:
    enum Bit
    { 
        kBitA, 
        kBitB, 
        kBitSelect, 
        kBitStart, 
        kBitRight,
        kBitLeft,
        kBitUp,
        kBitDown,
        kBitR,
        kBitL
    };

    void deinit();

    uint keyboardState() const;
    uint controllerState() const;

    SDL_GameController* controller = nullptr;
};

inline InputContext input_ctx;
