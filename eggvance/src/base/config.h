#pragma once

#include <shell/ini.h>

#include "filesystem.h"
#include "frontend/sdl2.h"
#include "gamepak/gpio.h"
#include "gamepak/save.h"

class Config
{
public:
    class RecentFileList : public std::vector<fs::path>
    {
    public:
        RecentFileList();

        bool hasFiles() const;

        void push(const fs::path& file);
    };

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

    ~Config();

    void init();

    fs::path       save_path;
    fs::path       bios_file;
    bool           bios_skip;
    RecentFileList recent;
    uint           fast_forward;
    Save::Type     save_type;
    Gpio::Type     gpio_type;
    uint           frame_size;
    bool           color_correct;
    bool           preserve_aspect_ratio;
    uint           video_layers;
    bool           mute;
    float          volume;
    uint           audio_channels;

    Controls<SDL_Scancode> keyboard;
    Controls<SDL_GameControllerButton> controller;

private:
    shell::Ini ini;
    bool initialized = false;
};

inline Config config;
