#pragma once

#include <SDL2/SDL_events.h>

#include "mmu/registers/keycontrol.h"

class Keypad
{
    friend class MMU;

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

private:
    struct IO
    {
        int keyinput;
        int keyinput_raw;
        KeyControl keycnt;
    } io;

    void processOpposingButtons(Button b1, Button b2);
    void processButton(Button button, bool pressed);

    SDL_GameController* controller;
};

extern Keypad keypad;
