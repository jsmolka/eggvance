#include "config.h"

#include "base/constants.h"
#include "base/exit.h"

template<>
std::optional<SDL_Scancode> eggcpt::parse(const std::string& data)
{
    if (data.empty())
        return SDL_SCANCODE_UNKNOWN;

    auto value = SDL_GetScancodeFromName(data.c_str());

    return value != SDL_SCANCODE_UNKNOWN
        ? std::optional(value)
        : std::nullopt;
}

template<>
std::optional<SDL_GameControllerButton> eggcpt::parse(const std::string& data)
{
    if (data.empty())
        return SDL_CONTROLLER_BUTTON_INVALID;

    auto value = SDL_GameControllerGetButtonFromString(data.c_str());

    return value != SDL_CONTROLLER_BUTTON_INVALID
        ? std::optional(value)
        : std::nullopt;
}

Config::Config(const fs::path& file)
{
    try
    {
        ini.load(file);
    }
    catch (const eggcpt::ParseError& error)
    {
        // Todo: log here
        eggcpt::reconstruct(ini);
    }

    save_path = ini.findOr("general", "save_path", fs::path());
    bios_file = ini.findOr("general", "bios_file", fs::path("bios.bin"));
    bios_skip = ini.findOr("general", "bios_skip", true);

    if (!save_path.empty()) save_path = fs::makeAbsolute(save_path);
    if (!bios_file.empty()) bios_file = fs::makeAbsolute(bios_file);

    fs::create_directories(save_path);

    save_type = ini.findOr("cartridge", "save_type", std::string("auto"));
    gpio_type = ini.findOr("cartridge", "gpio_type", std::string("auto"));

    eggcpt::toLower(save_type);
    eggcpt::toLower(gpio_type);

    if (!isValidSaveType(save_type)) exitWithMessage("Invalid config save_type: {}", save_type);
    if (!isValidGpioType(gpio_type)) exitWithMessage("Invalid config gpio_type: {}", gpio_type);

    framerate[0] = ini.findOr("framerate", "custom_1", 2.0 * kRefreshRate);
    framerate[1] = ini.findOr("framerate", "custom_2", 4.0 * kRefreshRate);
    framerate[2] = ini.findOr("framerate", "custom_3", 6.0 * kRefreshRate);
    framerate[3] = ini.findOr("framerate", "custom_4", 8.0 * kRefreshRate);

    controls.keyboard.a      = ini.findOr("controls_keyboard", "a",      SDL_SCANCODE_U);
    controls.keyboard.b      = ini.findOr("controls_keyboard", "b",      SDL_SCANCODE_H);
    controls.keyboard.up     = ini.findOr("controls_keyboard", "up",     SDL_SCANCODE_W);
    controls.keyboard.down   = ini.findOr("controls_keyboard", "down",   SDL_SCANCODE_S);
    controls.keyboard.left   = ini.findOr("controls_keyboard", "left",   SDL_SCANCODE_A);
    controls.keyboard.right  = ini.findOr("controls_keyboard", "right",  SDL_SCANCODE_D);
    controls.keyboard.start  = ini.findOr("controls_keyboard", "start",  SDL_SCANCODE_G);
    controls.keyboard.select = ini.findOr("controls_keyboard", "select", SDL_SCANCODE_F);
    controls.keyboard.l      = ini.findOr("controls_keyboard", "l",      SDL_SCANCODE_Q);
    controls.keyboard.r      = ini.findOr("controls_keyboard", "r",      SDL_SCANCODE_I);

    controls.controller.a      = ini.findOr("controls_controller", "a",      SDL_CONTROLLER_BUTTON_B);
    controls.controller.b      = ini.findOr("controls_controller", "b",      SDL_CONTROLLER_BUTTON_A);
    controls.controller.up     = ini.findOr("controls_controller", "up",     SDL_CONTROLLER_BUTTON_DPAD_UP);
    controls.controller.down   = ini.findOr("controls_controller", "down",   SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    controls.controller.left   = ini.findOr("controls_controller", "left",   SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    controls.controller.right  = ini.findOr("controls_controller", "right",  SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    controls.controller.start  = ini.findOr("controls_controller", "start",  SDL_CONTROLLER_BUTTON_START);
    controls.controller.select = ini.findOr("controls_controller", "select", SDL_CONTROLLER_BUTTON_BACK);
    controls.controller.l      = ini.findOr("controls_controller", "l",      SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    controls.controller.r      = ini.findOr("controls_controller", "r",      SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    shortcuts.keyboard.reset       = ini.findOr("shortcuts_keyboard", "reset",       SDL_SCANCODE_R);
    shortcuts.keyboard.fullscreen  = ini.findOr("shortcuts_keyboard", "fullscreen",  SDL_SCANCODE_F11);
    shortcuts.keyboard.fr_hardware = ini.findOr("shortcuts_keyboard", "fr_hardware", SDL_SCANCODE_1);
    shortcuts.keyboard.fr_custom_1 = ini.findOr("shortcuts_keyboard", "fr_custom_1", SDL_SCANCODE_2);
    shortcuts.keyboard.fr_custom_2 = ini.findOr("shortcuts_keyboard", "fr_custom_2", SDL_SCANCODE_3);
    shortcuts.keyboard.fr_custom_3 = ini.findOr("shortcuts_keyboard", "fr_custom_3", SDL_SCANCODE_4);
    shortcuts.keyboard.fr_custom_4 = ini.findOr("shortcuts_keyboard", "fr_custom_4", SDL_SCANCODE_5);
    shortcuts.keyboard.fr_unbound  = ini.findOr("shortcuts_keyboard", "fr_unbound",  SDL_SCANCODE_6);

    shortcuts.controller.reset       = ini.findOr("shortcuts_controller", "reset",       SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fullscreen  = ini.findOr("shortcuts_controller", "fullscreen",  SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fr_hardware = ini.findOr("shortcuts_controller", "fr_hardware", SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fr_custom_1 = ini.findOr("shortcuts_controller", "fr_custom_1", SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fr_custom_2 = ini.findOr("shortcuts_controller", "fr_custom_2", SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fr_custom_3 = ini.findOr("shortcuts_controller", "fr_custom_3", SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fr_custom_4 = ini.findOr("shortcuts_controller", "fr_custom_4", SDL_CONTROLLER_BUTTON_INVALID);
    shortcuts.controller.fr_unbound  = ini.findOr("shortcuts_controller", "fr_unbound",  SDL_CONTROLLER_BUTTON_INVALID);
}

bool Config::isValidSaveType(const std::string& type)
{
    return type == "auto"
        || type == "sram"
        || type == "flash64"
        || type == "flash128"
        || type == "eeprom";
}

bool Config::isValidGpioType(const std::string& type)
{
    return type == "auto"
        || type == "rtc";
}
