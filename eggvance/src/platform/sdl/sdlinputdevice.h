#pragma once

#include "platform.h"
#include "common/input.h"
#include "devices/inputdevice.h"

class SDLInputDevice : public InputDevice
{
public:
    ~SDLInputDevice();

    void init() override;
    void deinit() override;
    int state() override;

    void deviceEvent(const SDL_ControllerDeviceEvent& event);

    static SDL_Scancode convertKey(Key key);
    static SDL_GameControllerButton convertButton(Button button);

private:
    struct Controls
    {
        InputConfig<SDL_Scancode> keyboard;
        InputConfig<SDL_GameControllerButton> controller;
    } controls;

    int keyboardState() const;
    int controllerState() const;

    SDL_GameController* controller;
};
