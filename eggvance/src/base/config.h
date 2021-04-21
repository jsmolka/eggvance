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
    using iterator               = std::vector<fs::path>::iterator;
    using const_iterator         = std::vector<fs::path>::const_iterator;
    using reverse_iterator       = std::vector<fs::path>::reverse_iterator;
    using const_reverse_iterator = std::vector<fs::path>::const_reverse_iterator;

    RecentFiles();

    bool isEmpty() const;

    void clear();
    void push(const fs::path& file);

    SHELL_FORWARD_ITERATORS(files.begin(), files.end())
    SHELL_REVERSE_ITERATORS(files.end(), files.begin())

private:
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
