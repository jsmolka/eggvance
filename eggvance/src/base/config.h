#pragma once

#include "base/filesystem.h"
#include "base/sdl2.h"
#include "gamepak/gpio.h"
#include "gamepak/save.h"

template<typename Input>
struct Controls
{
    Input a;
    Input b;
    Input up;
    Input down;
    Input left;
    Input right;
    Input start;
    Input select;
    Input l;
    Input r;
};

template<typename Input>
struct Shortcuts
{
    Input reset;
    Input fullscreen;
    Input fr_hardware;
    Input fr_custom_1;
    Input fr_custom_2;
    Input fr_custom_3;
    Input fr_custom_4;
    Input fr_unbound;
};

class Config
{
public:
    void load(const fs::path& file);

    double framerate[4];
    fs::path save_path;
    fs::path bios_file;
    bool bios_hash;
    bool bios_skip;

    Save::Type save_type;
    Gpio::Type gpio_type;

    struct
    {
        Controls<SDL_Scancode> keyboard;
        Controls<SDL_GameControllerButton> controller;
    } controls;

    struct
    {
        Shortcuts<SDL_Scancode> keyboard;
        Shortcuts<SDL_GameControllerButton> controller;
    } shortcuts;
};

inline Config config;
