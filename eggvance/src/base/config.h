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

class RecentFileList : public std::vector<fs::path>
{
public:
    RecentFileList();

    bool hasFiles() const;

    void push(const fs::path& file);
};

class Ini
{
public:
    ~Ini();

    void init(const fs::path& file);

    template<typename T>
    T    get(const std::string& section, const std::string& key) const;
    void set(const std::string& section, const std::string& key, const std::string& value);

    shell::Ini ini;
private:
    shell::filesystem::path file;
    bool changed = false;
};

class Config : private Ini
{
public:
    ~Config();

    void init(const fs::path& file);

    // New
    RecentFileList recent;
    uint           fast_forward;
    Save::Type     save_type;
    Gpio::Type     gpio_type;

    // Old
    fs::path   save_path;
    fs::path   bios_file;
    bool       bios_skip;
    bool       bios_hash;
    bool       lcd_color;
    double     volume;
    double     volume_step;

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
