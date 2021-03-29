#pragma once

#include <shell/ini.h>

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
    Input pause;
    Input fullscreen;
    Input volume_up;
    Input volume_down;
    Input speed_hardware;
    Input speed_2x;
    Input speed_4x;
    Input speed_6x;
    Input speed_8x;
    Input speed_unbound;
};

class Config
{
public:
    void init(const fs::path& file);
    void deinit();

    fs::path bios_file;
    bool skip_bios;
    bool validate_bios;
    bool emulate_lcd;
    double volume;
    double volume_step;

    fs::path save_path;
    bool sync_save;
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

private:
    template<typename T>
    T find(const std::string& section, const std::string& key) const;

    fs::path file;
    shell::Ini ini;
};

inline Config config;
