#pragma once

#if defined (_MSC_VER) || defined (__EMSCRIPTEN__)
#include <SDL2/SDL.h>
#else
#include "SDL.h"
#endif

#include "base/input.h"
#include "platform/inputdevice.h"

class SDLInputDevice : public InputDevice
{
public:
    ~SDLInputDevice();

    void init() override;
    void deinit() override;
    uint state() override;

    void processDeviceEvent(const SDL_ControllerDeviceEvent& event);

    static SDL_Scancode convertKey(Key key);
    static SDL_GameControllerButton convertButton(Button button);

private:
    struct
    {
        Controls<SDL_Scancode> keyboard;
        Controls<SDL_GameControllerButton> controller;
    } controls;
    
    SDL_GameController* controller;

    uint keyboardState() const;
    uint controllerState() const;
};
