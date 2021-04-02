#include "config.h"

#include <shell/errors.h>

#include "ppu/constants.h"

constexpr std::string_view kFallbackContent = R"(
[general]
# Relative or absolute save path
# An empty value stores save files next to the ROM
save_path =
# Relative or absolute BIOS file path
# An empty value uses the replacement BIOS
bios_file =
# Skip the BIOS intro sequence
bios_skip = true
# Validate the BIOS hash
bios_hash = true
# Emulate LCD colors
lcd_color = true
# Volume between 0 and 1
volume = 0.5
# Volume step
volume_step = 0.1

[gamepak]
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
up = DpUp
down = DpDown
left = DpLeft
right = DpRight
start = Start
select = Back
l = LeftShoulder
r = RightShoulder

[controller_shortcuts]
# Possible values: https://smolka.dev/sdl/controller/
reset =
pause =
fullscreen =
volume_up =
volume_down =
speed_hardware =
speed_2x =
speed_4x =
speed_6x =
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

Ini::~Ini()
{
    if (file.empty() || !changed)
        return;

    std::error_code ec;
    fs::create_directories(file.parent_path(), ec);
    
    if (ini.save(file) != fs::Status::Ok)
        showMessageBox("Warning", "Cannot write config: {}", file);
}

void Ini::init(const fs::path& file)
{
    std::string content;
    if (fs::read(file, content) != fs::Status::Ok)
    {
        changed = true;
        content = kFallbackContent;
    }

    try
    {
        ini.parse(content);
    }
    catch (const shell::ParseError& error)
    {
        throw shell::Error("Cannot parse config: {}", error.what());
    }

    this->file = file;
}

template<typename T>
T Ini::get(const std::string& section, const std::string& key) const
{
    if (const auto data = ini.find<std::string>(section, key))
    {
        if (const auto value = shell::parse<T>(*data))
            return *value;

        throw shell::Error("Cannot parse config value '{}' of key '{}.{}'", *data, section, key);
    }
    throw shell::Error("Cannot find config value for key '{}.{}'", section, key);
}

void Ini::set(const std::string& section, const std::string& key, const std::string& value)
{
    changed |= get<std::string>(section, key) != value;

    ini.set(section, key, value);
}

Config::~Config()
{
    set("general", "volume", shell::format(volume));
}

void Config::init(const fs::path& file)
{
    Ini::init(file);

    save_path   = get<fs::path  >("general", "save_path");
    bios_file   = get<fs::path  >("general", "bios_file");
    bios_skip   = get<bool      >("general", "bios_skip");
    bios_hash   = get<bool      >("general", "bios_hash");
    lcd_color   = get<bool      >("general", "lcd_color");
    volume      = get<double    >("general", "volume");
    volume_step = get<double    >("general", "volume_step");
    save_type   = get<Save::Type>("gamepak", "save_type");
    gpio_type   = get<Gpio::Type>("gamepak", "gpio_type");

    if (!save_path.empty()) save_path = fs::absolute(save_path);
    if (!bios_file.empty()) bios_file = fs::absolute(bios_file);

    controls.keyboard.a      = get<SDL_Scancode>("keyboard_controls", "a");
    controls.keyboard.b      = get<SDL_Scancode>("keyboard_controls", "b");
    controls.keyboard.up     = get<SDL_Scancode>("keyboard_controls", "up");
    controls.keyboard.down   = get<SDL_Scancode>("keyboard_controls", "down");
    controls.keyboard.left   = get<SDL_Scancode>("keyboard_controls", "left");
    controls.keyboard.right  = get<SDL_Scancode>("keyboard_controls", "right");
    controls.keyboard.start  = get<SDL_Scancode>("keyboard_controls", "start");
    controls.keyboard.select = get<SDL_Scancode>("keyboard_controls", "select");
    controls.keyboard.l      = get<SDL_Scancode>("keyboard_controls", "l");
    controls.keyboard.r      = get<SDL_Scancode>("keyboard_controls", "r");

    shortcuts.keyboard.reset          = get<SDL_Scancode>("keyboard_shortcuts", "reset");
    shortcuts.keyboard.pause          = get<SDL_Scancode>("keyboard_shortcuts", "pause");
    shortcuts.keyboard.volume_up      = get<SDL_Scancode>("keyboard_shortcuts", "volume_up");
    shortcuts.keyboard.volume_down    = get<SDL_Scancode>("keyboard_shortcuts", "volume_down");
    shortcuts.keyboard.fullscreen     = get<SDL_Scancode>("keyboard_shortcuts", "fullscreen");
    shortcuts.keyboard.speed_hardware = get<SDL_Scancode>("keyboard_shortcuts", "speed_hardware");
    shortcuts.keyboard.speed_2x       = get<SDL_Scancode>("keyboard_shortcuts", "speed_2x");
    shortcuts.keyboard.speed_4x       = get<SDL_Scancode>("keyboard_shortcuts", "speed_4x");
    shortcuts.keyboard.speed_6x       = get<SDL_Scancode>("keyboard_shortcuts", "speed_6x");
    shortcuts.keyboard.speed_8x       = get<SDL_Scancode>("keyboard_shortcuts", "speed_8x");
    shortcuts.keyboard.speed_unbound  = get<SDL_Scancode>("keyboard_shortcuts", "speed_unbound");

    controls.controller.a      = get<SDL_GameControllerButton>("controller_controls", "a");
    controls.controller.b      = get<SDL_GameControllerButton>("controller_controls", "b");
    controls.controller.up     = get<SDL_GameControllerButton>("controller_controls", "up");
    controls.controller.down   = get<SDL_GameControllerButton>("controller_controls", "down");
    controls.controller.left   = get<SDL_GameControllerButton>("controller_controls", "left");
    controls.controller.right  = get<SDL_GameControllerButton>("controller_controls", "right");
    controls.controller.start  = get<SDL_GameControllerButton>("controller_controls", "start");
    controls.controller.select = get<SDL_GameControllerButton>("controller_controls", "select");
    controls.controller.l      = get<SDL_GameControllerButton>("controller_controls", "l");
    controls.controller.r      = get<SDL_GameControllerButton>("controller_controls", "r");

    shortcuts.controller.reset          = get<SDL_GameControllerButton>("controller_shortcuts", "reset");
    shortcuts.controller.pause          = get<SDL_GameControllerButton>("controller_shortcuts", "pause");
    shortcuts.controller.volume_up      = get<SDL_GameControllerButton>("controller_shortcuts", "volume_up");
    shortcuts.controller.volume_down    = get<SDL_GameControllerButton>("controller_shortcuts", "volume_down");
    shortcuts.controller.fullscreen     = get<SDL_GameControllerButton>("controller_shortcuts", "fullscreen");
    shortcuts.controller.speed_hardware = get<SDL_GameControllerButton>("controller_shortcuts", "speed_hardware");
    shortcuts.controller.speed_2x       = get<SDL_GameControllerButton>("controller_shortcuts", "speed_2x");
    shortcuts.controller.speed_4x       = get<SDL_GameControllerButton>("controller_shortcuts", "speed_4x");
    shortcuts.controller.speed_6x       = get<SDL_GameControllerButton>("controller_shortcuts", "speed_6x");
    shortcuts.controller.speed_8x       = get<SDL_GameControllerButton>("controller_shortcuts", "speed_8x");
    shortcuts.controller.speed_unbound  = get<SDL_GameControllerButton>("controller_shortcuts", "speed_unbound");
}
