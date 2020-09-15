#pragma once

#include <eggcpt/ini.h>

#include "base/filesystem.h"
#include "base/sdl2.h"

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
    Config() = default;
    Config(const fs::path& file);

    bool bios_skip;
    fs::path bios_file;
    fs::path save_path;
    double framerate[4];

    std::string save_type;
    std::string gpio_type;

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

private:
    static bool isValidSaveType(const std::string& type);
    static bool isValidGpioType(const std::string& type);

    eggcpt::Ini ini;
};

inline Config config;
