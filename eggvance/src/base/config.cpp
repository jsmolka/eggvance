#include "config.h"

#include <toml/toml.h>

#include "base/constants.h"

Config config;

void Config::init(int argc, char* argv[])
{
    try
    {
        initFile(fs::u8path(argv[0]).parent_path() / "eggvance.toml");
    }
    catch (const std::exception&)
    {
        initDefault();
    }
}

void Config::initFile(const fs::path& file)
{
    auto stream = std::ifstream(file);
    auto result = toml::parse(stream);
    if (!result.valid())
        throw std::exception();

    const auto toml = result.value;

    save_path = fs::u8path(toml.get<std::string>("general.save_path"));
    bios_file = fs::u8path(toml.get<std::string>("general.bios_file"));
    bios_skip = toml.get<bool>("general.bios_skip");

    save_type = toml.get<std::string>("cartridge.save_type");
    gpio_type = toml.get<std::string>("cartridge.gpio_type");

    if (!save_path.empty())
    {
        if (save_path.is_relative())
            save_path = file.parent_path() / save_path;

        if (!fs::is_directory(save_path))
            fs::create_directories(save_path);
    }

    if (!bios_file.empty())
    {
        if (bios_file.is_relative())
            bios_file = file.parent_path() / bios_file;
    }

    framerate[0] = toml.get<double>("framerate.custom_1");
    framerate[1] = toml.get<double>("framerate.custom_2");
    framerate[2] = toml.get<double>("framerate.custom_3");
    framerate[3] = toml.get<double>("framerate.custom_4");

    const auto a      = toml.get<std::vector<std::string>>("controls.a");
    const auto b      = toml.get<std::vector<std::string>>("controls.b");
    const auto up     = toml.get<std::vector<std::string>>("controls.up");
    const auto down   = toml.get<std::vector<std::string>>("controls.down");
    const auto left   = toml.get<std::vector<std::string>>("controls.left");
    const auto right  = toml.get<std::vector<std::string>>("controls.right");
    const auto start  = toml.get<std::vector<std::string>>("controls.start");
    const auto select = toml.get<std::vector<std::string>>("controls.select");
    const auto l      = toml.get<std::vector<std::string>>("controls.l");
    const auto r      = toml.get<std::vector<std::string>>("controls.r");

    controls.keyboard.a      = SDL_GetScancodeFromName(a[0].c_str());
    controls.keyboard.b      = SDL_GetScancodeFromName(b[0].c_str());
    controls.keyboard.up     = SDL_GetScancodeFromName(up[0].c_str());
    controls.keyboard.down   = SDL_GetScancodeFromName(down[0].c_str());
    controls.keyboard.left   = SDL_GetScancodeFromName(left[0].c_str());
    controls.keyboard.right  = SDL_GetScancodeFromName(right[0].c_str());
    controls.keyboard.start  = SDL_GetScancodeFromName(start[0].c_str());
    controls.keyboard.select = SDL_GetScancodeFromName(select[0].c_str());
    controls.keyboard.l      = SDL_GetScancodeFromName(l[0].c_str());
    controls.keyboard.r      = SDL_GetScancodeFromName(r[0].c_str());

    controls.controller.a      = SDL_GameControllerGetButtonFromString(a[1].c_str());
    controls.controller.b      = SDL_GameControllerGetButtonFromString(b[1].c_str());
    controls.controller.up     = SDL_GameControllerGetButtonFromString(up[1].c_str());
    controls.controller.down   = SDL_GameControllerGetButtonFromString(down[1].c_str());
    controls.controller.left   = SDL_GameControllerGetButtonFromString(left[1].c_str());
    controls.controller.right  = SDL_GameControllerGetButtonFromString(right[1].c_str());
    controls.controller.start  = SDL_GameControllerGetButtonFromString(start[1].c_str());
    controls.controller.select = SDL_GameControllerGetButtonFromString(select[1].c_str());
    controls.controller.l      = SDL_GameControllerGetButtonFromString(l[1].c_str());
    controls.controller.r      = SDL_GameControllerGetButtonFromString(r[1].c_str());

    const auto reset       = toml.get<std::vector<std::string>>("shortcuts.reset");
    const auto fullscreen  = toml.get<std::vector<std::string>>("shortcuts.fullscreen");
    const auto fr_hardware = toml.get<std::vector<std::string>>("shortcuts.fr_hardware");
    const auto fr_custom_1 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_1");
    const auto fr_custom_2 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_2");
    const auto fr_custom_3 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_3");
    const auto fr_custom_4 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_4");
    const auto fr_unbound  = toml.get<std::vector<std::string>>("shortcuts.fr_unbound");

    shortcuts.keyboard.reset       = SDL_GetScancodeFromName(reset[0].c_str());
    shortcuts.keyboard.fullscreen  = SDL_GetScancodeFromName(fullscreen[0].c_str());
    shortcuts.keyboard.fr_hardware = SDL_GetScancodeFromName(fr_hardware[0].c_str());
    shortcuts.keyboard.fr_custom_1 = SDL_GetScancodeFromName(fr_custom_1[0].c_str());
    shortcuts.keyboard.fr_custom_2 = SDL_GetScancodeFromName(fr_custom_2[0].c_str());
    shortcuts.keyboard.fr_custom_3 = SDL_GetScancodeFromName(fr_custom_3[0].c_str());
    shortcuts.keyboard.fr_custom_4 = SDL_GetScancodeFromName(fr_custom_4[0].c_str());
    shortcuts.keyboard.fr_unbound  = SDL_GetScancodeFromName(fr_unbound[0].c_str());

    shortcuts.controller.reset       = SDL_GameControllerGetButtonFromString(reset[1].c_str());
    shortcuts.controller.fullscreen  = SDL_GameControllerGetButtonFromString(fullscreen[1].c_str());
    shortcuts.controller.fr_hardware = SDL_GameControllerGetButtonFromString(fr_hardware[1].c_str());
    shortcuts.controller.fr_custom_1 = SDL_GameControllerGetButtonFromString(fr_custom_1[1].c_str());
    shortcuts.controller.fr_custom_2 = SDL_GameControllerGetButtonFromString(fr_custom_2[1].c_str());
    shortcuts.controller.fr_custom_3 = SDL_GameControllerGetButtonFromString(fr_custom_3[1].c_str());
    shortcuts.controller.fr_custom_4 = SDL_GameControllerGetButtonFromString(fr_custom_4[1].c_str());
    shortcuts.controller.fr_unbound  = SDL_GameControllerGetButtonFromString(fr_unbound[1].c_str());
}

void Config::initDefault()
{
    save_path = fs::path();
    bios_file = fs::path();
    bios_skip = true;

    save_type = "auto";
    gpio_type = "auto";

    framerate[0] = 2.0 * kRefreshRate;
    framerate[1] = 4.0 * kRefreshRate;
    framerate[2] = 6.0 * kRefreshRate;
    framerate[3] = 8.0 * kRefreshRate;

    controls.keyboard    = decltype(controls.keyboard)();
    controls.controller  = decltype(controls.controller)();
    shortcuts.keyboard   = decltype(shortcuts.keyboard)();
    shortcuts.controller = decltype(shortcuts.controller)();
}
