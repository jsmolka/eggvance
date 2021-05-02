#pragma once

#include <shell/ini.h>
#include <shell/ranges.h>

#include "filesystem.h"
#include "base/int.h"
#include "frontend/sdl2.h"

template<typename Input>
class Controls
{
public:
    void unassign(Input input)
    {
        auto none = Input(-1);

        if (a      == input) a      = none;
        if (b      == input) b      = none;
        if (up     == input) up     = none;
        if (down   == input) down   = none;
        if (left   == input) left   = none;
        if (right  == input) right  = none;
        if (start  == input) start  = none;
        if (select == input) select = none;
        if (l      == input) l      = none;
        if (r      == input) r      = none;
    }

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

class RecentFiles
{
public:
    using iterator       = std::vector<fs::path>::iterator;
    using const_iterator = std::vector<fs::path>::const_iterator;

    RecentFiles();

    bool isEmpty() const;

    void clear();
    void push(const fs::path& file);

    SHELL_FORWARD_ITERATORS(files.begin(), files.end())

private:
    static constexpr auto kSize = 10;

    std::vector<fs::path> files;
};

class Ini : public shell::Ini
{
public:
    ~Ini();

    void init();

private:
    static std::optional<fs::path> file();

    bool initialized = false;
};

class Config : public Ini
{
public:
    ~Config();

    void init();

    fs::path    save_path;
    fs::path    bios_file;
    bool        bios_skip;
    RecentFiles recent;
    uint        fast_forward;
    uint        frame_size;
    bool        color_correct;
    bool        preserve_aspect_ratio;
    bool        mute;
    float       volume;
    uint        video_layers;
    uint        audio_channels;

    Controls<SDL_Scancode> keyboard;
    Controls<SDL_GameControllerButton> controller;
};

inline Config config;

