#pragma once

#include "base/int.h"
#include "base/sdl2.h"

class InputContext
{
public:
    ~InputContext();

    void init();
    uint state() const;
    void handleDeviceEvent(const SDL_ControllerDeviceEvent& event);

private:
    enum class Bit
    { 
        A,
        B,
        Select,
        Start,
        Right,
        Left,
        Up,
        Down,
        R,
        L
    };

    uint keyboardState() const;
    uint controllerState() const;

    SDL_GameController* controller = nullptr;
};

inline InputContext input_ctx;
