#include "config.h"

#include <shell/ini.h>

#include "panic.h"
#include "ppu/constants.h"

constexpr std::string_view kConfig = R"(
[general]
# Relative or absolute BIOS file path
# An empty value uses the replacement BIOS
bios_file = C:/Users/Julian/OneDrive/Dev/gba/bios/bios.bin
# Skip the BIOS intro sequence
skip_bios = true
# Validate the BIOS
validate_bios = true
# Emulate LCD colors
emulate_lcd = true
# Volume between 0 and 1
volume = 0.5
# Volume step
volume_step = 0.1

[gamepak]
# Relative or absolute save path
# An empty value stores save files next to the ROM
save_path = C:/Users/Julian/OneDrive/Dev/gba/saves
# Sync internal state and save file
sync_save = true
# Force a cartridge save type
# Possible values: detect, none, sram, eeprom, flash512, flash1024
save_type = detect
# Force a cartridge GPIO type
# Possible values: detect, none, rtc
gpio_type = detect

[keyboard_controls]
# Possible values: https://smolka.dev/sdl/keyboard/
a = U
b = H
up = W
down = S
left = A
right = D
start = G
select = F
l = Q
r = I

[keyboard_shortcuts]
# Possible values: https://smolka.dev/sdl/keyboard/
reset = R
pause = P
fullscreen = F11
volume_up = Up
volume_down = Down
speed_hardware = 1
speed_2x = 2
speed_4x = 3
speed_6x = 4
speed_8x = 5
speed_unbound = 6

[controller_controls]
# Possible values: https://smolka.dev/sdl/controller/
a = B
b = A
up =
down =
left =
right =
start = Start
select = Back
l = LeftShoulder
r = RightShoulder

[controller_shortcuts]
# Possible values: https://smolka.dev/sdl/controller/
reset =
pause = DpDown
fullscreen = DpUp
volume_up =
volume_down =
speed_hardware = DpLeft
speed_2x =
speed_4x =
speed_6x = DpRight
speed_8x =
speed_unbound =
)";

template<>
std::optional<SDL_Scancode> shell::parse(const std::string& data)
{
    if (data.empty())
        return SDL_SCANCODE_UNKNOWN;

    const auto scancode = SDL_GetScancodeFromName(data.c_str());
    return scancode != SDL_SCANCODE_UNKNOWN
        ? std::optional(scancode)
        : std::nullopt;
}

template<>
std::optional<SDL_GameControllerButton> shell::parse(const std::string& data)
{
    if (data.empty())
        return SDL_CONTROLLER_BUTTON_INVALID;

    const auto button = SDL_GameControllerGetButtonFromString(data.c_str());
    return button != SDL_CONTROLLER_BUTTON_INVALID
        ? std::optional(button)
        : std::nullopt;
}

template<>
std::optional<Save::Type> shell::parse(const std::string& data)
{
    const auto type = shell::toLowerCopy(data);

    if (type == "detect")    return Save::Type::Detect;
    if (type == "none")      return Save::Type::None;
    if (type == "sram")      return Save::Type::Sram;
    if (type == "flash512")  return Save::Type::Flash512;
    if (type == "flash1024") return Save::Type::Flash1024;
    if (type == "eeprom")    return Save::Type::Eeprom;

    return std::nullopt;
}

template<>
std::optional<Gpio::Type> shell::parse(const std::string& data)
{
    const auto type = shell::toLowerCopy(data);

    if (type == "detect") return Gpio::Type::Detect;
    if (type == "none")   return Gpio::Type::None;
    if (type == "rtc")    return Gpio::Type::Rtc;

    return std::nullopt;
}

void Config::init(const fs::path& file)
{
    this->file = file;

    std::string data;
    if (fs::read(file, data) != fs::Status::Ok)
        data = kConfig;

    try
    {
        ini.parse(data);
    }
    catch (const shell::ParseError& error)
    {
        panic("Cannot parse config {}\n{}", file, error.what());
    }

    bios_file     = find<fs::path>("general", "bios_file");
    skip_bios     = find<bool>("general", "skip_bios");
    validate_bios = find<bool>("general", "validate_bios");
    emulate_lcd   = find<bool>("general", "emulate_lcd");
    volume        = find<double>("general", "volume");
    volume_step   = find<double>("general", "volume_step");

    save_path = find<fs::path>("gamepak", "save_path");
    sync_save = find<bool>("gamepak", "sync_save");
    save_type = find<Save::Type>("gamepak", "save_type");
    gpio_type = find<Gpio::Type>("gamepak", "gpio_type");

    if (!save_path.empty())
    {
        save_path = fs::absolute(save_path);

        if (!fs::is_directory(save_path))
        {
            if (!fs::create_directories(save_path))
                panic("Cannot create save path {}", save_path);
        }
    }

    if (!bios_file.empty())
    {
        bios_file = fs::absolute(bios_file);
    }
    
    controls.keyboard.a      = find<SDL_Scancode>("keyboard_controls", "a");
    controls.keyboard.b      = find<SDL_Scancode>("keyboard_controls", "b");
    controls.keyboard.up     = find<SDL_Scancode>("keyboard_controls", "up");
    controls.keyboard.down   = find<SDL_Scancode>("keyboard_controls", "down");
    controls.keyboard.left   = find<SDL_Scancode>("keyboard_controls", "left");
    controls.keyboard.right  = find<SDL_Scancode>("keyboard_controls", "right");
    controls.keyboard.start  = find<SDL_Scancode>("keyboard_controls", "start");
    controls.keyboard.select = find<SDL_Scancode>("keyboard_controls", "select");
    controls.keyboard.l      = find<SDL_Scancode>("keyboard_controls", "l");
    controls.keyboard.r      = find<SDL_Scancode>("keyboard_controls", "r");

    shortcuts.keyboard.reset          = find<SDL_Scancode>("keyboard_shortcuts", "reset");
    shortcuts.keyboard.pause          = find<SDL_Scancode>("keyboard_shortcuts", "pause");
    shortcuts.keyboard.volume_up      = find<SDL_Scancode>("keyboard_shortcuts", "volume_up");
    shortcuts.keyboard.volume_down    = find<SDL_Scancode>("keyboard_shortcuts", "volume_down");
    shortcuts.keyboard.fullscreen     = find<SDL_Scancode>("keyboard_shortcuts", "fullscreen");
    shortcuts.keyboard.speed_hardware = find<SDL_Scancode>("keyboard_shortcuts", "speed_hardware");
    shortcuts.keyboard.speed_2x       = find<SDL_Scancode>("keyboard_shortcuts", "speed_2x");
    shortcuts.keyboard.speed_4x       = find<SDL_Scancode>("keyboard_shortcuts", "speed_4x");
    shortcuts.keyboard.speed_6x       = find<SDL_Scancode>("keyboard_shortcuts", "speed_6x");
    shortcuts.keyboard.speed_8x       = find<SDL_Scancode>("keyboard_shortcuts", "speed_8x");
    shortcuts.keyboard.speed_unbound  = find<SDL_Scancode>("keyboard_shortcuts", "speed_unbound");

    controls.controller.a      = find<SDL_GameControllerButton>("controller_controls", "a");
    controls.controller.b      = find<SDL_GameControllerButton>("controller_controls", "b");
    controls.controller.up     = find<SDL_GameControllerButton>("controller_controls", "up");
    controls.controller.down   = find<SDL_GameControllerButton>("controller_controls", "down");
    controls.controller.left   = find<SDL_GameControllerButton>("controller_controls", "left");
    controls.controller.right  = find<SDL_GameControllerButton>("controller_controls", "right");
    controls.controller.start  = find<SDL_GameControllerButton>("controller_controls", "start");
    controls.controller.select = find<SDL_GameControllerButton>("controller_controls", "select");
    controls.controller.l      = find<SDL_GameControllerButton>("controller_controls", "l");
    controls.controller.r      = find<SDL_GameControllerButton>("controller_controls", "r");

    shortcuts.controller.reset          = find<SDL_GameControllerButton>("controller_shortcuts", "reset");
    shortcuts.controller.pause          = find<SDL_GameControllerButton>("controller_shortcuts", "pause");
    shortcuts.controller.volume_up      = find<SDL_GameControllerButton>("controller_shortcuts", "volume_up");
    shortcuts.controller.volume_down    = find<SDL_GameControllerButton>("controller_shortcuts", "volume_down");
    shortcuts.controller.fullscreen     = find<SDL_GameControllerButton>("controller_shortcuts", "fullscreen");
    shortcuts.controller.speed_hardware = find<SDL_GameControllerButton>("controller_shortcuts", "speed_hardware");
    shortcuts.controller.speed_2x       = find<SDL_GameControllerButton>("controller_shortcuts", "speed_2x");
    shortcuts.controller.speed_4x       = find<SDL_GameControllerButton>("controller_shortcuts", "speed_4x");
    shortcuts.controller.speed_6x       = find<SDL_GameControllerButton>("controller_shortcuts", "speed_6x");
    shortcuts.controller.speed_8x       = find<SDL_GameControllerButton>("controller_shortcuts", "speed_8x");
    shortcuts.controller.speed_unbound  = find<SDL_GameControllerButton>("controller_shortcuts", "speed_unbound");
}

void Config::deinit()
{
    if (!file.empty())
    {
        std::string volume = std::to_string(this->volume);
        volume.erase(volume.find_last_not_of('0') + 1, std::string::npos);
        volume.erase(volume.find_last_not_of('.') + 1, std::string::npos);

        ini.set("general", "volume", volume);
        ini.save(file);
    }
}

template<typename T>
T Config::find(const std::string& section, const std::string& key) const
{
    if (const auto data = ini.find<std::string>(section, key))
    {
        if (const auto value = shell::parse<T>(*data))
            return *value;

        panic("Cannot parse config value {} of key {}.{}", *data, section, key);
    }
    panic("Cannot find config key {}.{}", section, key);

    return T();
}
