#pragma once

#include "base/bit.h"
#include "base/filesystem.h"
#include "base/int.h"
#include "base/sdl2.h"

template<typename T>
struct Controls {};

template<>
struct Controls<SDL_Scancode>
{
    SDL_Scancode a      = SDL_SCANCODE_U;
    SDL_Scancode b      = SDL_SCANCODE_H;
    SDL_Scancode up     = SDL_SCANCODE_W;
    SDL_Scancode down   = SDL_SCANCODE_S;
    SDL_Scancode left   = SDL_SCANCODE_A;
    SDL_Scancode right  = SDL_SCANCODE_D;
    SDL_Scancode start  = SDL_SCANCODE_G;
    SDL_Scancode select = SDL_SCANCODE_F;
    SDL_Scancode l      = SDL_SCANCODE_Q;
    SDL_Scancode r      = SDL_SCANCODE_I;
};

template<>
struct Controls<SDL_GameControllerButton>
{
    SDL_GameControllerButton a      = SDL_CONTROLLER_BUTTON_B;
    SDL_GameControllerButton b      = SDL_CONTROLLER_BUTTON_A;
    SDL_GameControllerButton up     = SDL_CONTROLLER_BUTTON_DPAD_UP;
    SDL_GameControllerButton down   = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    SDL_GameControllerButton left   = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    SDL_GameControllerButton right  = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    SDL_GameControllerButton start  = SDL_CONTROLLER_BUTTON_START;
    SDL_GameControllerButton select = SDL_CONTROLLER_BUTTON_BACK;
    SDL_GameControllerButton l      = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    SDL_GameControllerButton r      = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
};

template<typename T>
struct Shortcuts {};

template<>
struct Shortcuts<SDL_Scancode>
{
    SDL_Scancode reset       = SDL_SCANCODE_R;
    SDL_Scancode fullscreen  = SDL_SCANCODE_F11;
    SDL_Scancode fr_hardware = SDL_SCANCODE_1;
    SDL_Scancode fr_custom_1 = SDL_SCANCODE_2;
    SDL_Scancode fr_custom_2 = SDL_SCANCODE_3;
    SDL_Scancode fr_custom_3 = SDL_SCANCODE_4;
    SDL_Scancode fr_custom_4 = SDL_SCANCODE_5;
    SDL_Scancode fr_unbound  = SDL_SCANCODE_6;
};

template<>
struct Shortcuts<SDL_GameControllerButton>
{
    SDL_GameControllerButton reset       = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fullscreen  = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fr_hardware = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fr_custom_1 = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fr_custom_2 = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fr_custom_3 = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fr_custom_4 = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerButton fr_unbound  = SDL_CONTROLLER_BUTTON_INVALID;
};

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
