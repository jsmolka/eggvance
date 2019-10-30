#pragma once

#include <string>
#include <unordered_map>
#include <SDL2/SDL_keyboard.h>

#include "sys/keypad.h"

class Config
{
public:
    enum class Shortcut
    {
        Reset,
        Fullscreen,
        SpeedDefault,
        SpeedOption1,
        SpeedOption2,
        SpeedOption3,
        SpeedOption4,
        SpeedUnlimited
    };

    void init();

    int deadzone;
    bool bios_skip;
    std::string bios_file;
    std::string save_dir;

    struct Controls
    {
        std::unordered_map<SDL_Keycode, Keypad::Button> keyboard;
        std::unordered_map<SDL_GameControllerButton, Keypad::Button> controller;
    } controls;

    struct Shortcuts
    {
        std::unordered_map<SDL_Keycode, Shortcut> keyboard;
        std::unordered_map<SDL_GameControllerButton, Shortcut> controller;
    } shortcuts;

    double fps_multipliers[4];

private:
    void initFile();
    void initDefault();
};

extern Config config;
