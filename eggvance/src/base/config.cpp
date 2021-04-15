#include "config.h"

#include <shell/errors.h>

template<typename T>
std::optional<T> parseEnum(const std::string& data)
{
    if (const auto value = shell::parse<uint>(data))
        return static_cast<T>(*value);

    return std::nullopt;
}

template<> std::optional<SDL_Scancode>             shell::parse(const std::string& data) { return parseEnum<SDL_Scancode>(data); }
template<> std::optional<SDL_GameControllerButton> shell::parse(const std::string& data) { return parseEnum<SDL_GameControllerButton>(data); }
template<> std::optional<Save::Type>               shell::parse(const std::string& data) { return parseEnum<Save::Type>(data); }
template<> std::optional<Gpio::Type>               shell::parse(const std::string& data) { return parseEnum<Gpio::Type>(data); }

Config::~Config()
{
    if (!initialized)
        return;

    for (auto [index, file] : shell::enumerate(recent))
    {
        ini.set("file", shell::format("recent_{}", index), file.u8string());
    }

    ini.set("settings",   "save_path",             shell::format(save_path));
    ini.set("settings",   "bios_file",             shell::format(bios_file));
    ini.set("settings",   "bios_skip",             shell::format(bios_skip));
    ini.set("emulation",  "fast_forward",          shell::format(fast_forward));
    ini.set("emulation",  "save_type",             shell::format(uint(save_type)));
    ini.set("emulation",  "gpio_type",             shell::format(uint(gpio_type)));
    ini.set("video",      "frame_size",            shell::format(frame_size));
    ini.set("video",      "color_correct",         shell::format(color_correct));
    ini.set("video",      "preserve_aspect_ratio", shell::format(preserve_aspect_ratio));
    ini.set("video",      "video_layers",          shell::format(video_layers));
    ini.set("audio",      "mute",                  shell::format(mute));
    ini.set("audio",      "volume",                shell::format(volume));
    ini.set("audio",      "audio_channels",        shell::format(audio_channels));
    ini.set("keyboard",   "a",                     shell::format(keyboard.a));
    ini.set("keyboard",   "b",                     shell::format(keyboard.b));
    ini.set("keyboard",   "up",                    shell::format(keyboard.up));
    ini.set("keyboard",   "down",                  shell::format(keyboard.down));
    ini.set("keyboard",   "left",                  shell::format(keyboard.left));
    ini.set("keyboard",   "right",                 shell::format(keyboard.right));
    ini.set("keyboard",   "start",                 shell::format(keyboard.start));
    ini.set("keyboard",   "select",                shell::format(keyboard.select));
    ini.set("keyboard",   "l",                     shell::format(keyboard.l));
    ini.set("keyboard",   "r",                     shell::format(keyboard.r));
    ini.set("controller", "a",                     shell::format(controller.a));
    ini.set("controller", "b",                     shell::format(controller.b));
    ini.set("controller", "up",                    shell::format(controller.up));
    ini.set("controller", "down",                  shell::format(controller.down));
    ini.set("controller", "left",                  shell::format(controller.left));
    ini.set("controller", "right",                 shell::format(controller.right));
    ini.set("controller", "start",                 shell::format(controller.start));
    ini.set("controller", "select",                shell::format(controller.select));
    ini.set("controller", "l",                     shell::format(controller.l));
    ini.set("controller", "r",                     shell::format(controller.r));

    if (char* path = SDL_GetPrefPath("eggvance", "eggvance"))
    {
        try
        {
            ini.save(fs::u8path(path) / "eggvance.ini");
        }
        catch (const std::exception&) {}

        SDL_free(path);
    }
}

void Config::init()
{
    if (char* path = SDL_GetPrefPath("eggvance", "eggvance"))
    {
        try
        {
            ini.load(fs::u8path(path) / "eggvance.ini");
        }
        catch (const std::exception&) {}

        SDL_free(path);
    }

    for (auto [index, file] : shell::enumerate(recent))
    {
        file = ini.findOr("file", shell::format("recent_{}", index), fs::path());
    }

    save_path             = ini.findOr("settings",   "save_path",             fs::path());
    bios_file             = ini.findOr("settings",   "bios_file",             fs::path());
    bios_skip             = ini.findOr("settings",   "bios_skip",             true);
    fast_forward          = ini.findOr("emulation",  "fast_forward",          2);
    save_type             = ini.findOr("emulation",  "save_type",             Save::Type::Detect);
    gpio_type             = ini.findOr("emulation",  "gpio_type",             Gpio::Type::Detect);
    frame_size            = ini.findOr("video",      "frame_size",            4);
    color_correct         = ini.findOr("video",      "color_correct",         true);
    preserve_aspect_ratio = ini.findOr("video",      "preserve_aspect_ratio", true);
    video_layers          = ini.findOr("video",      "video_layers",          0b11111);
    mute                  = ini.findOr("audio",      "mute",                  false);
    volume                = ini.findOr("audio",      "volume",                0.5);
    audio_channels        = ini.findOr("audio",      "audio_channels",        0b111111);
    keyboard.a            = ini.findOr("keyboard",   "a",                     SDL_SCANCODE_U);
    keyboard.b            = ini.findOr("keyboard",   "b",                     SDL_SCANCODE_H);
    keyboard.up           = ini.findOr("keyboard",   "up",                    SDL_SCANCODE_W);
    keyboard.down         = ini.findOr("keyboard",   "down",                  SDL_SCANCODE_S);
    keyboard.left         = ini.findOr("keyboard",   "left",                  SDL_SCANCODE_A);
    keyboard.right        = ini.findOr("keyboard",   "right",                 SDL_SCANCODE_D);
    keyboard.start        = ini.findOr("keyboard",   "start",                 SDL_SCANCODE_G);
    keyboard.select       = ini.findOr("keyboard",   "select",                SDL_SCANCODE_F);
    keyboard.l            = ini.findOr("keyboard",   "l",                     SDL_SCANCODE_Q);
    keyboard.r            = ini.findOr("keyboard",   "r",                     SDL_SCANCODE_I);
    controller.a          = ini.findOr("controller", "a",                     SDL_CONTROLLER_BUTTON_B);
    controller.b          = ini.findOr("controller", "b",                     SDL_CONTROLLER_BUTTON_A);
    controller.up         = ini.findOr("controller", "up",                    SDL_CONTROLLER_BUTTON_DPAD_UP);
    controller.down       = ini.findOr("controller", "down",                  SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    controller.left       = ini.findOr("controller", "left",                  SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    controller.right      = ini.findOr("controller", "right",                 SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    controller.start      = ini.findOr("controller", "start",                 SDL_CONTROLLER_BUTTON_START);
    controller.select     = ini.findOr("controller", "select",                SDL_CONTROLLER_BUTTON_BACK);
    controller.l          = ini.findOr("controller", "l",                     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    controller.r          = ini.findOr("controller", "r",                     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    initialized = true;
}

void Config::RecentFileList::push(const fs::path& file)
{
    auto iter = std::find(begin(), end(), file);
    if ( iter != end())
        erase(iter);

    if (size() == capacity())
        pop_back();

    insert(begin(), file);
}

Config::RecentFileList::RecentFileList()
{
    resize(10);
}

bool Config::RecentFileList::hasFiles() const
{
    for (const auto& file : *this)
    {
        if (!file.empty())
            return true;
    }
    return false;
}
