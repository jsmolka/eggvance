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
    enum { kA, kB, kSelect, kStart, kRight, kLeft, kUp, kDown, kR, kL };

    void deinit();

    uint keyboardState() const;
    uint controllerState() const;

    SDL_GameController* controller = nullptr;
};

inline InputContext input_ctx;
