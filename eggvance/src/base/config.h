#pragma once

// Todo: move to core

#include "base/fs.h"
#include "base/input.h"
#include "base/sdl2.h"

class Config
{
public:
    void init(int argc, char* argv[]);

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

    double framerate[4];
    fs::path save_path;
    fs::path bios_file;
    bool bios_skip;

    std::string save_type;
    std::string gpio_type;

private:
    void initFile(const fs::path& file);
    void initDefault();
};

extern Config config;
