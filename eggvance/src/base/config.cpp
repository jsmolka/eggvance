#include "config.h"

#include <iostream>

#include <eggcpt/algorithm.h>

#include "base/constants.h"
#include "base/exit.h"

Config::Config(const fs::path& file)
    : data(toml::parse(std::ifstream(file)).value)
{
    using namespace std::string_literals;

    save_path = fs::u8path(find("general.save_path", ""s));
    bios_file = fs::u8path(find("general.bios_file", "bios.bin"s));
    bios_skip = find("general.bios_skip", true);

    if (!save_path.empty()) save_path = fs::makeAbsolute(save_path);
    if (!bios_file.empty()) bios_file = fs::makeAbsolute(bios_file);

    fs::create_directories(save_path);

    save_type = find("cartridge.save_type", "auto"s);
    gpio_type = find("cartridge.gpio_type", "auto"s);

    eggcpt::toLower(save_type);
    eggcpt::toLower(gpio_type);

    if (!isValidSaveType(save_type)) exitWithMessage("Invalid config save_type: {}", save_type);
    if (!isValidGpioType(gpio_type)) exitWithMessage("Invalid config gpio_type: {}", gpio_type);

    framerate[0] = find("framerate.custom_1", 2.0 * kRefreshRate);
    framerate[1] = find("framerate.custom_2", 4.0 * kRefreshRate);
    framerate[2] = find("framerate.custom_3", 6.0 * kRefreshRate);
    framerate[3] = find("framerate.custom_4", 8.0 * kRefreshRate);

    controls.keyboard.a      = findKey("controls.keyboard.a", "U");
    controls.keyboard.b      = findKey("controls.keyboard.b", "H");
    controls.keyboard.up     = findKey("controls.keyboard.up", "W");
    controls.keyboard.down   = findKey("controls.keyboard.down", "S");
    controls.keyboard.left   = findKey("controls.keyboard.left", "A");
    controls.keyboard.right  = findKey("controls.keyboard.right", "D");
    controls.keyboard.start  = findKey("controls.keyboard.start", "G");
    controls.keyboard.select = findKey("controls.keyboard.select", "F");
    controls.keyboard.l      = findKey("controls.keyboard.l", "Q");
    controls.keyboard.r      = findKey("controls.keyboard.r", "I");

    controls.controller.a      = findButton("controls.controller.a", "B");
    controls.controller.b      = findButton("controls.controller.b", "A");
    controls.controller.up     = findButton("controls.controller.up", "DpUp");
    controls.controller.down   = findButton("controls.controller.down", "DpDown");
    controls.controller.left   = findButton("controls.controller.left", "DpLeft");
    controls.controller.right  = findButton("controls.controller.right", "DpRight");
    controls.controller.start  = findButton("controls.controller.start", "Start");
    controls.controller.select = findButton("controls.controller.select", "Back");
    controls.controller.l      = findButton("controls.controller.l", "LeftShoulder");
    controls.controller.r      = findButton("controls.controller.r", "RightShoulder");

    shortcuts.keyboard.reset       = findKey("shortcuts.keyboard.reset", "R");
    shortcuts.keyboard.fullscreen  = findKey("shortcuts.keyboard.fullscreen", "F11");
    shortcuts.keyboard.fr_hardware = findKey("shortcuts.keyboard.fr_hardware", "1");
    shortcuts.keyboard.fr_custom_1 = findKey("shortcuts.keyboard.fr_custom_1", "2");
    shortcuts.keyboard.fr_custom_2 = findKey("shortcuts.keyboard.fr_custom_2", "3");
    shortcuts.keyboard.fr_custom_3 = findKey("shortcuts.keyboard.fr_custom_3", "4");
    shortcuts.keyboard.fr_custom_4 = findKey("shortcuts.keyboard.fr_custom_4", "5");
    shortcuts.keyboard.fr_unbound  = findKey("shortcuts.keyboard.fr_unbound", "6");

    shortcuts.controller.reset       = findButton("shortcuts.controller.reset", "");
    shortcuts.controller.fullscreen  = findButton("shortcuts.controller.fullscreen", "");
    shortcuts.controller.fr_hardware = findButton("shortcuts.controller.fr_hardware", "");
    shortcuts.controller.fr_custom_1 = findButton("shortcuts.controller.fr_custom_1", "");
    shortcuts.controller.fr_custom_2 = findButton("shortcuts.controller.fr_custom_2", "");
    shortcuts.controller.fr_custom_3 = findButton("shortcuts.controller.fr_custom_3", "");
    shortcuts.controller.fr_custom_4 = findButton("shortcuts.controller.fr_custom_4", "");
    shortcuts.controller.fr_unbound  = findButton("shortcuts.controller.fr_unbound", "");
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

template<typename T>
T Config::find(const std::string& key, const T& fallback) const
{
    const toml::Value* value = data.find(key);

    if (value && value->is<T>())
        return value->as<T>();
    else
        return fallback;
}

SDL_Scancode Config::findKey(const std::string& key, const std::string& fallback) const
{
    return SDL_GetScancodeFromName(
        find(key, fallback).c_str());
}

SDL_GameControllerButton Config::findButton(const std::string& key, const std::string& fallback) const
{
    return SDL_GameControllerGetButtonFromString(
        find(key, fallback).c_str());
}
