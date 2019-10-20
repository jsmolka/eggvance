#pragma once

#include <string>
#include <SDL2/SDL_keyboard.h>
#include <unordered_map>

#include "keypad.h"

class Config
{
public:
    void init(const std::string& dir);

    int deadzone;
    bool bios_skip;
    std::string bios_file;
    std::unordered_map<SDL_Keycode, Keypad::Button> keyboard_map;
    std::unordered_map<SDL_GameControllerButton, Keypad::Button> controller_map;
};

extern Config config;
