#pragma once

#include "common/defines.h"
#include "common/input.h"
#include "platform/inputdevice.h"

#if COMPILER_MSVC || COMPILER_EMSCRIPTEN
#include <SDL2/SDL.h>
#else
#include "SDL.h"
#endif

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
    struct Controls
    {
        InputConfig<SDL_Scancode> keyboard;
        InputConfig<SDL_GameControllerButton> controller;
    } controls;
    
    SDL_GameController* controller;

    uint keyboardState() const;
    uint controllerState() const;
};
