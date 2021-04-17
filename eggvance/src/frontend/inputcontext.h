#pragma once

#include "sdl2.h"
#include "base/int.h"

class InputContext
{
public:
    ~InputContext();

    void init();
    void update();
    uint state() const;

    void deviceEvent(const SDL_ControllerDeviceEvent& event);

private:
    enum class Bit { A, B, Select, Start, Right, Left, Up, Down, R, L };

    uint keyboardState() const;
    uint controllerAxisState() const;
    uint controllerButtonState() const;

    uint keyboard_state = 0;
    uint controller_state = 0;
    SDL_GameController* controller = nullptr;
};

inline InputContext input_ctx;
