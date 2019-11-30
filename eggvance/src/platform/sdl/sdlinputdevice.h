#pragma once

#include <SDL2/SDL.h>

#include "common/input.h"
#include "devices/inputdevice.h"

class SDLInputDevice : public InputDevice
{
public:
    SDLInputDevice();
    ~SDLInputDevice();

    void init() override;
    void deinit() override;
    void poll(u16& state) override;

private:
    struct
    {
        SDL_Scancode a;
        SDL_Scancode b;
        SDL_Scancode up;
        SDL_Scancode down;
        SDL_Scancode left;
        SDL_Scancode right;
        SDL_Scancode start;
        SDL_Scancode select;
        SDL_Scancode l;
        SDL_Scancode r;
    } keyboard;

    struct
    {
        SDL_GameControllerButton a;
        SDL_GameControllerButton b;
        SDL_GameControllerButton up;
        SDL_GameControllerButton down;
        SDL_GameControllerButton left;
        SDL_GameControllerButton right;
        SDL_GameControllerButton start;
        SDL_GameControllerButton select;
        SDL_GameControllerButton l;
        SDL_GameControllerButton r;
    } controller;

    static SDL_Scancode mapKey(Key key);
};
