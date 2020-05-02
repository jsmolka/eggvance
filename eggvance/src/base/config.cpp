#include "config.h"

#include <toml/toml.h>

#include "base/constants.h"

Config config;

void Config::init(int argc, char* argv[])
{
    if (argc > 0)
        parent = fs::u8path(argv[0]).parent_path();

    try
    {
        auto config = parent / "eggvance.toml";
        if (fs::exists(config))
            initFile(config);
        else
            initDefault();
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

    auto& value = result.value;

    save_path = fs::u8path(value.get<std::string>("general.save_path"));
    bios_file = fs::u8path(value.get<std::string>("general.bios_file"));
    bios_skip = value.get<bool>("general.bios_skip");

    if (bios_file.is_relative() && !bios_file.empty())
        bios_file = parent / bios_file;

    if (!save_path.empty())
    {
        if (save_path.is_relative())
            save_path = parent / save_path;

        if (!fs::is_directory(save_path))
            fs::create_directories(save_path);
    }

    framerate[0] = value.get<double>("framerate.custom_1");
    framerate[1] = value.get<double>("framerate.custom_2");
    framerate[2] = value.get<double>("framerate.custom_3");
    framerate[3] = value.get<double>("framerate.custom_4");

    const auto a      = value.get<std::vector<std::string>>("controls.a"     );
    const auto b      = value.get<std::vector<std::string>>("controls.b"     );
    const auto up     = value.get<std::vector<std::string>>("controls.up"    );
    const auto down   = value.get<std::vector<std::string>>("controls.down"  );
    const auto left   = value.get<std::vector<std::string>>("controls.left"  );
    const auto right  = value.get<std::vector<std::string>>("controls.right" );
    const auto start  = value.get<std::vector<std::string>>("controls.start" );
    const auto select = value.get<std::vector<std::string>>("controls.select");
    const auto l      = value.get<std::vector<std::string>>("controls.l"     );
    const auto r      = value.get<std::vector<std::string>>("controls.r"     );

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

    const auto reset       = value.get<std::vector<std::string>>("shortcuts.reset"      );
    const auto fullscreen  = value.get<std::vector<std::string>>("shortcuts.fullscreen" );
    const auto fr_hardware = value.get<std::vector<std::string>>("shortcuts.fr_hardware");
    const auto fr_custom_1 = value.get<std::vector<std::string>>("shortcuts.fr_custom_1");
    const auto fr_custom_2 = value.get<std::vector<std::string>>("shortcuts.fr_custom_2");
    const auto fr_custom_3 = value.get<std::vector<std::string>>("shortcuts.fr_custom_3");
    const auto fr_custom_4 = value.get<std::vector<std::string>>("shortcuts.fr_custom_4");
    const auto fr_unbound  = value.get<std::vector<std::string>>("shortcuts.fr_unbound" );

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

    framerate[0] = 2.0 * kRefreshRate;
    framerate[1] = 4.0 * kRefreshRate;
    framerate[2] = 6.0 * kRefreshRate;
    framerate[3] = 8.0 * kRefreshRate;

    controls.keyboard.a      = KEY_U;
    controls.keyboard.b      = KEY_H;
    controls.keyboard.up     = KEY_W;
    controls.keyboard.down   = KEY_S;
    controls.keyboard.left   = KEY_A;
    controls.keyboard.right  = KEY_D;
    controls.keyboard.start  = KEY_G;
    controls.keyboard.select = KEY_F;
    controls.keyboard.l      = KEY_Q;
    controls.keyboard.r      = KEY_I;

    controls.controller.a      = BTN_B;
    controls.controller.b      = BTN_A;
    controls.controller.up     = BTN_UP;
    controls.controller.down   = BTN_DOWN;
    controls.controller.left   = BTN_LEFT;
    controls.controller.right  = BTN_RIGHT;
    controls.controller.start  = BTN_START;
    controls.controller.select = BTN_BACK;
    controls.controller.l      = BTN_L;
    controls.controller.r      = BTN_R;

    shortcuts.keyboard.reset       = KEY_R;
    shortcuts.keyboard.fullscreen  = KEY_F11;
    shortcuts.keyboard.fr_hardware = KEY_1;
    shortcuts.keyboard.fr_custom_1 = KEY_2;
    shortcuts.keyboard.fr_custom_2 = KEY_3;
    shortcuts.keyboard.fr_custom_3 = KEY_4;
    shortcuts.keyboard.fr_custom_4 = KEY_5;
    shortcuts.keyboard.fr_unbound  = KEY_6;

    shortcuts.controller.reset       = BTN_NONE;
    shortcuts.controller.fullscreen  = BTN_NONE;
    shortcuts.controller.fr_hardware = BTN_NONE;
    shortcuts.controller.fr_custom_1 = BTN_NONE;
    shortcuts.controller.fr_custom_2 = BTN_NONE;
    shortcuts.controller.fr_custom_3 = BTN_NONE;
    shortcuts.controller.fr_custom_4 = BTN_NONE;
    shortcuts.controller.fr_unbound  = BTN_NONE;
}
