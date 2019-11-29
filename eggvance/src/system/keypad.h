#pragma once

#include <SDL2/SDL_events.h>

#include "registers/keycontrol.h"
#include "registers/keyinput.h"

class Keypad
{
public:
    enum class Button
    {
        A      = 1 << 0,
        B      = 1 << 1,
        Select = 1 << 2,
        Start  = 1 << 3,
        Right  = 1 << 4,
        Left   = 1 << 5,
        Up     = 1 << 6,
        Down   = 1 << 7,
        R      = 1 << 8,
        L      = 1 << 9
    };

    ~Keypad();

    void reset();

    bool init();

    void keyboardEvent(const SDL_KeyboardEvent& event);
    void controllerAxisEvent(const SDL_ControllerAxisEvent& event);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event);
    void controllerDeviceEvent(const SDL_ControllerDeviceEvent& event);

    struct IO
    {
        KeyControl keycnt;
        KeyInput keyinput;
        KeyInput keyinput_raw;
    } io;

private:
    void handleInput(Button button, bool pressed);
    void handleOppositeInputs(Button b1, Button b2);

    SDL_GameController* controller;
};

extern Keypad keypad;
