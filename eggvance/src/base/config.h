#pragma once

#include "filesystem.h"
#include "sdl2.h"
#include "gamepak/gpio.h"
#include "gamepak/save.h"

template<typename Input>
class Controls
{
public:
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
class Shortcuts
{
public:
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
    void init(const fs::path& file);

    fs::path save_path;
    fs::path bios_file;
    bool bios_skip;
    bool bios_hash;

    Save::Type save;
    Gpio::Type gpio;
    double framerate[4];

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
