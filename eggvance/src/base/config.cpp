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

    controls.keyboard.a      = keyByName(a[0]);
    controls.keyboard.b      = keyByName(b[0]);
    controls.keyboard.up     = keyByName(up[0]);
    controls.keyboard.down   = keyByName(down[0]);
    controls.keyboard.left   = keyByName(left[0]);
    controls.keyboard.right  = keyByName(right[0]);
    controls.keyboard.start  = keyByName(start[0]);
    controls.keyboard.select = keyByName(select[0]);
    controls.keyboard.l      = keyByName(l[0]);
    controls.keyboard.r      = keyByName(r[0]);

    controls.controller.a      = buttonByName(a[1]);
    controls.controller.b      = buttonByName(b[1]);
    controls.controller.up     = buttonByName(up[1]);
    controls.controller.down   = buttonByName(down[1]);
    controls.controller.left   = buttonByName(left[1]);
    controls.controller.right  = buttonByName(right[1]);
    controls.controller.start  = buttonByName(start[1]);
    controls.controller.select = buttonByName(select[1]);
    controls.controller.l      = buttonByName(l[1]);
    controls.controller.r      = buttonByName(r[1]);

    const auto reset       = toml.get<std::vector<std::string>>("shortcuts.reset");
    const auto fullscreen  = toml.get<std::vector<std::string>>("shortcuts.fullscreen");
    const auto fr_hardware = toml.get<std::vector<std::string>>("shortcuts.fr_hardware");
    const auto fr_custom_1 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_1");
    const auto fr_custom_2 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_2");
    const auto fr_custom_3 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_3");
    const auto fr_custom_4 = toml.get<std::vector<std::string>>("shortcuts.fr_custom_4");
    const auto fr_unbound  = toml.get<std::vector<std::string>>("shortcuts.fr_unbound");

    shortcuts.keyboard.reset       = keyByName(reset[0]);
    shortcuts.keyboard.fullscreen  = keyByName(fullscreen[0]);
    shortcuts.keyboard.fr_hardware = keyByName(fr_hardware[0]);
    shortcuts.keyboard.fr_custom_1 = keyByName(fr_custom_1[0]);
    shortcuts.keyboard.fr_custom_2 = keyByName(fr_custom_2[0]);
    shortcuts.keyboard.fr_custom_3 = keyByName(fr_custom_3[0]);
    shortcuts.keyboard.fr_custom_4 = keyByName(fr_custom_4[0]);
    shortcuts.keyboard.fr_unbound  = keyByName(fr_unbound[0]);

    shortcuts.controller.reset       = buttonByName(reset[1]);
    shortcuts.controller.fullscreen  = buttonByName(fullscreen[1]);
    shortcuts.controller.fr_hardware = buttonByName(fr_hardware[1]);
    shortcuts.controller.fr_custom_1 = buttonByName(fr_custom_1[1]);
    shortcuts.controller.fr_custom_2 = buttonByName(fr_custom_2[1]);
    shortcuts.controller.fr_custom_3 = buttonByName(fr_custom_3[1]);
    shortcuts.controller.fr_custom_4 = buttonByName(fr_custom_4[1]);
    shortcuts.controller.fr_unbound  = buttonByName(fr_unbound[1]);
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

    controls.keyboard.a      = Key::U;
    controls.keyboard.b      = Key::H;
    controls.keyboard.up     = Key::W;
    controls.keyboard.down   = Key::S;
    controls.keyboard.left   = Key::A;
    controls.keyboard.right  = Key::D;
    controls.keyboard.start  = Key::G;
    controls.keyboard.select = Key::F;
    controls.keyboard.l      = Key::Q;
    controls.keyboard.r      = Key::I;

    controls.controller.a      = Button::B;
    controls.controller.b      = Button::A;
    controls.controller.up     = Button::Up;
    controls.controller.down   = Button::Down;
    controls.controller.left   = Button::Left;
    controls.controller.right  = Button::Right;
    controls.controller.start  = Button::Start;
    controls.controller.select = Button::Back;
    controls.controller.l      = Button::L;
    controls.controller.r      = Button::R;

    shortcuts.keyboard.reset       = Key::R;
    shortcuts.keyboard.fullscreen  = Key::F11;
    shortcuts.keyboard.fr_hardware = Key::N1;
    shortcuts.keyboard.fr_custom_1 = Key::N2;
    shortcuts.keyboard.fr_custom_2 = Key::N3;
    shortcuts.keyboard.fr_custom_3 = Key::N4;
    shortcuts.keyboard.fr_custom_4 = Key::N5;
    shortcuts.keyboard.fr_unbound  = Key::N6;

    shortcuts.controller.reset       = Button::None;
    shortcuts.controller.fullscreen  = Button::None;
    shortcuts.controller.fr_hardware = Button::None;
    shortcuts.controller.fr_custom_1 = Button::None;
    shortcuts.controller.fr_custom_2 = Button::None;
    shortcuts.controller.fr_custom_3 = Button::None;
    shortcuts.controller.fr_custom_4 = Button::None;
    shortcuts.controller.fr_unbound  = Button::None;
}
