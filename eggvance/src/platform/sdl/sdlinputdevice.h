#pragma once

#include <SDL2/SDL.h>

#include "common/input.h"
#include "devices/inputdevice.h"

class SDLInputDevice : public InputDevice
{
public:
    ~SDLInputDevice();

    void init() override;
    void deinit() override;
    void poll(u16& state) override;

    void deviceEvent(const SDL_ControllerDeviceEvent& event);

    static SDL_Scancode mapKey(Key key);
    static SDL_GameControllerButton mapButton(Button button);

private:
    struct
    {
        InputConfig<SDL_Scancode> keyboard;
        InputConfig<SDL_GameControllerButton> controller;
    } map;

    void pollKeys(u16& state);
    void pollButtons(u16& state);

    SDL_GameController* controller;
};
