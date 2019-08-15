#pragma once

#include <SDL2/SDL_events.h>

#include "mmu/mmu.h"

class Input
{
public:
    Input(MMU& mmu);
    ~Input();


    void keyEvent(const SDL_KeyboardEvent& event);

    void controllerAxisEvent(const SDL_ControllerAxisEvent& event);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event);
    void controllerDeviceEvent(const SDL_ControllerDeviceEvent& event);

private:
    enum Button
    {
        BTN_A      = 1 << 0,
        BTN_B      = 1 << 1,
        BTN_SELECT = 1 << 2,
        BTN_START  = 1 << 3,
        BTN_RIGHT  = 1 << 4,
        BTN_LEFT   = 1 << 5,
        BTN_UP     = 1 << 6,
        BTN_DOWN   = 1 << 7,
        BTN_R      = 1 << 8,
        BTN_L      = 1 << 9
    };

    bool isButtonPressed(Button button) const;
    void processInput(Button button, int state);

    MMU& mmu;

    SDL_GameController* controller;
};
