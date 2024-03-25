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
        set("recent", fmt::format("file_{}", index), reinterpret_cast<const char*>(file.u8string().c_str()));
    }

    set("settings",   "save_path",             fmt::to_string(save_path));
    set("settings",   "bios_file",             fmt::to_string(bios_file));
    set("settings",   "bios_skip",             fmt::to_string(bios_skip));
    set("emulation",  "fast_forward",          fmt::to_string(fast_forward));
    set("video",      "frame_size",            fmt::to_string(frame_size));
    set("video",      "color_correct",         fmt::to_string(color_correct));
    set("video",      "preserve_aspect_ratio", fmt::to_string(preserve_aspect_ratio));
    set("audio",      "mute",                  fmt::to_string(mute));
    set("audio",      "volume",                fmt::to_string(volume));
    set("video",      "video_layers",          fmt::to_string(video_layers));
    set("audio",      "audio_channels",        fmt::to_string(audio_channels));
    set("keyboard",   "a",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.a)));
    set("keyboard",   "b",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.b)));
    set("keyboard",   "up",                    fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.up)));
    set("keyboard",   "down",                  fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.down)));
    set("keyboard",   "left",                  fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.left)));
    set("keyboard",   "right",                 fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.right)));
    set("keyboard",   "start",                 fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.start)));
    set("keyboard",   "select",                fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.select)));
    set("keyboard",   "l",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.l)));
    set("keyboard",   "r",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_Scancode>>(keyboard.r)));
    set("controller", "a",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.a)));
    set("controller", "b",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.b)));
    set("controller", "up",                    fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.up)));
    set("controller", "down",                  fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.down)));
    set("controller", "left",                  fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.left)));
    set("controller", "right",                 fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.right)));
    set("controller", "start",                 fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.start)));
    set("controller", "select",                fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.select)));
    set("controller", "l",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.l)));
    set("controller", "r",                     fmt::to_string(static_cast<std::underlying_type_t<SDL_GameControllerButton>>(controller.r)));
}

void Config::init()
{
    Ini::init();

    for (int index = 9; index >= 0; --index)
    {
        recent.push(findOr("recent", fmt::format("file_{}", index), fs::path()));
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
