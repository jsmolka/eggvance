#include "config.h"

template<typename Enum>
std::optional<Enum> parseEnum(const std::string& data)
{
    static_assert(std::is_enum_v<Enum>);

    if (const auto value = shell::parse<std::underlying_type_t<Enum>>(data))
        return static_cast<Enum>(*value);

    return std::nullopt;
}

template<>
std::optional<SDL_Scancode> shell::parse(const std::string& data)
{
    return parseEnum<SDL_Scancode>(data);
}

template<>
std::optional<SDL_GameControllerButton> shell::parse(const std::string& data)
{
    return parseEnum<SDL_GameControllerButton>(data);
}

RecentFiles::RecentFiles()
{
    files.resize(kSize, fs::path());
}

bool RecentFiles::isEmpty() const
{
    for (const auto& file : files)
    {
        if (!file.empty())
            return false;
    }
    return true;
}

void RecentFiles::clear()
{
    std::fill(files.begin(), files.end(), fs::path());
}

void RecentFiles::push(const fs::path& file)
{
    if (file.empty() || !fs::is_regular_file(file))
        return;

    auto copy(file);
    copy.make_preferred();

    auto iter = std::find(files.begin(), files.end(), file);
    if ( iter != files.end())
        files.erase(iter);
    else
        files.pop_back();

    files.insert(files.begin(), copy);
}

Ini::~Ini()
{
    if (initialized)
    {
        if (const auto file = this->file())
            save(*file);
    }
}

void Ini::init()
{
    try
    {
        if (const auto file = this->file())
            load(*file);
    }
    catch (const shell::ParseError&) {}

    initialized = true;
}

std::optional<fs::path> Ini::file()
{
    if (char* path = SDL_GetPrefPath("jsmolka", "eggvance"))
    {
        const auto file = fs::u8path(path) / "eggvance.ini";
        SDL_free(path);

        return file;
    }
    return std::nullopt;
}

Config::~Config()
{
    for (auto [index, file] : shell::enumerate(recent))
    {
        set("recent", shell::format("file_{}", index), file.u8string());
    }

    set("settings",   "save_path",             shell::format(save_path));
    set("settings",   "bios_file",             shell::format(bios_file));
    set("settings",   "bios_skip",             shell::format(bios_skip));
    set("emulation",  "fast_forward",          shell::format(fast_forward));
    set("video",      "frame_size",            shell::format(frame_size));
    set("video",      "color_correct",         shell::format(color_correct));
    set("video",      "preserve_aspect_ratio", shell::format(preserve_aspect_ratio));
    set("audio",      "mute",                  shell::format(mute));
    set("audio",      "volume",                shell::format(volume));
    set("video",      "video_layers",          shell::format(video_layers));
    set("audio",      "audio_channels",        shell::format(audio_channels));
    set("keyboard",   "a",                     shell::format(keyboard.a));
    set("keyboard",   "b",                     shell::format(keyboard.b));
    set("keyboard",   "up",                    shell::format(keyboard.up));
    set("keyboard",   "down",                  shell::format(keyboard.down));
    set("keyboard",   "left",                  shell::format(keyboard.left));
    set("keyboard",   "right",                 shell::format(keyboard.right));
    set("keyboard",   "start",                 shell::format(keyboard.start));
    set("keyboard",   "select",                shell::format(keyboard.select));
    set("keyboard",   "l",                     shell::format(keyboard.l));
    set("keyboard",   "r",                     shell::format(keyboard.r));
    set("controller", "a",                     shell::format(controller.a));
    set("controller", "b",                     shell::format(controller.b));
    set("controller", "up",                    shell::format(controller.up));
    set("controller", "down",                  shell::format(controller.down));
    set("controller", "left",                  shell::format(controller.left));
    set("controller", "right",                 shell::format(controller.right));
    set("controller", "start",                 shell::format(controller.start));
    set("controller", "select",                shell::format(controller.select));
    set("controller", "l",                     shell::format(controller.l));
    set("controller", "r",                     shell::format(controller.r));
}

void Config::init()
{
    Ini::init();

    for (int index = 9; index >= 0; --index)
    {
        recent.push(findOr("recent", shell::format("file_{}", index), fs::path()));
    }

    save_path             = findOr("settings",   "save_path",             fs::path());
    bios_file             = findOr("settings",   "bios_file",             fs::path());
    bios_skip             = findOr("settings",   "bios_skip",             true);
    fast_forward          = findOr("emulation",  "fast_forward",          1'000'000);
    frame_size            = findOr("video",      "frame_size",            4);
    color_correct         = findOr("video",      "color_correct",         true);
    preserve_aspect_ratio = findOr("video",      "preserve_aspect_ratio", true);
    mute                  = findOr("audio",      "mute",                  false);
    volume                = findOr("audio",      "volume",                0.5);
    video_layers          = findOr("video",      "video_layers",          0b11111);
    audio_channels        = findOr("audio",      "audio_channels",        0b111111);
    keyboard.a            = findOr("keyboard",   "a",                     SDL_SCANCODE_U);
    keyboard.b            = findOr("keyboard",   "b",                     SDL_SCANCODE_H);
    keyboard.up           = findOr("keyboard",   "up",                    SDL_SCANCODE_W);
    keyboard.down         = findOr("keyboard",   "down",                  SDL_SCANCODE_S);
    keyboard.left         = findOr("keyboard",   "left",                  SDL_SCANCODE_A);
    keyboard.right        = findOr("keyboard",   "right",                 SDL_SCANCODE_D);
    keyboard.start        = findOr("keyboard",   "start",                 SDL_SCANCODE_G);
    keyboard.select       = findOr("keyboard",   "select",                SDL_SCANCODE_F);
    keyboard.l            = findOr("keyboard",   "l",                     SDL_SCANCODE_Q);
    keyboard.r            = findOr("keyboard",   "r",                     SDL_SCANCODE_I);
    controller.a          = findOr("controller", "a",                     SDL_CONTROLLER_BUTTON_B);
    controller.b          = findOr("controller", "b",                     SDL_CONTROLLER_BUTTON_A);
    controller.up         = findOr("controller", "up",                    SDL_CONTROLLER_BUTTON_DPAD_UP);
    controller.down       = findOr("controller", "down",                  SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    controller.left       = findOr("controller", "left",                  SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    controller.right      = findOr("controller", "right",                 SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    controller.start      = findOr("controller", "start",                 SDL_CONTROLLER_BUTTON_START);
    controller.select     = findOr("controller", "select",                SDL_CONTROLLER_BUTTON_BACK);
    controller.l          = findOr("controller", "l",                     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    controller.r          = findOr("controller", "r",                     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}
