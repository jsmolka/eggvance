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
    if (const auto value = shell::parse<uint>(data))
        return Save::Type(*value);

    return std::nullopt;
}

template<>
std::optional<Gpio::Type> shell::parse(const std::string& data)
{
    if (const auto value = shell::parse<uint>(data))
        return Gpio::Type(*value);

    return std::nullopt;
}

Ini::~Ini()
{
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
    for (auto [index, file] : shell::enumerate(recent))
    {
        ini.set("file", shell::format("recent_{}", index), file.u8string());
    }

    ini.set("emulation", "fast_forward",          shell::format(fast_forward));
    ini.set("emulation", "save_type",             shell::format(uint(save_type)));
    ini.set("emulation", "gpio_type",             shell::format(uint(gpio_type)));
    ini.set("video",     "frame_size",            shell::format(frame_size));
    ini.set("video",     "color_correct",         shell::format(color_correct));
    ini.set("video",     "preserve_aspect_ratio", shell::format(preserve_aspect_ratio));
    ini.set("audio",     "mute",                  shell::format(mute));
    ini.set("audio",     "volume",                shell::format(volume));
}

void Config::init(const fs::path& file)
{
    Ini::init(file);

    for (auto [index, file] : shell::enumerate(recent))
    {
        file = ini.findOr("file", shell::format("recent_{}", index), fs::path());
    }

    fast_forward          = ini.findOr("emulation", "fast_forward",          2);
    save_type             = ini.findOr("emulation", "save_type",             Save::Type::Detect);
    gpio_type             = ini.findOr("emulation", "gpio_type",             Gpio::Type::Detect);
    frame_size            = ini.findOr("video",     "frame_size",            2);
    color_correct         = ini.findOr("video",     "color_correct",         true);
    preserve_aspect_ratio = ini.findOr("video",     "preserve_aspect_ratio", true);
    mute                  = ini.findOr("audio",     "mute",                  false);
    volume                = ini.findOr("audio",     "volume",                0.5);

    save_path   = get<fs::path  >("general", "save_path");
    bios_file   = get<fs::path  >("general", "bios_file");
    bios_skip   = get<bool      >("general", "bios_skip");
    bios_hash   = get<bool      >("general", "bios_hash");
    lcd_color   = get<bool      >("general", "lcd_color");
    volume_step = get<double    >("general", "volume_step");

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

void RecentFileList::push(const fs::path& file)
{
    auto iter = std::find(begin(), end(), file);
    if ( iter != end())
        erase(iter);

    if (size() == capacity())
        pop_back();

    insert(begin(), file);
}

RecentFileList::RecentFileList()
{
    resize(10);
}

bool RecentFileList::hasFiles() const
{
    for (const auto& file : *this)
    {
        if (!file.empty())
            return true;
    }
    return false;
}
