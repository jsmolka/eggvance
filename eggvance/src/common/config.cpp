#include "config.h"

#include <toml/toml.h>

Config config;

void Config::init()
{
    try
    {
        initFile();
    }
    catch (std::exception ex)
    {
        initDefault();
    }
}

void Config::initFile()
{
    auto stream = std::ifstream(fs::relativeToExe("eggvance.toml"));
    auto result = toml::parse(stream);
    if (!result.valid())
        throw std::exception();

    auto& value = result.value;

    bios_file = value.get<std::string>("general.bios_file");
    save_dir  = value.get<std::string>("general.save_dir");
    bios_skip = value.get<bool>("general.bios_skip");
    deadzone  = value.get<int>("general.deadzone");

    if (!bios_file.empty() && bios_file.is_relative())
        bios_file = fs::relativeToExe(bios_file);

    if (!save_dir.empty())
    {
        if (save_dir.is_relative())
            save_dir = fs::relativeToExe(save_dir);

        if (!fs::isDir(save_dir))
            std::filesystem::create_directories(save_dir);
    }

    fps_multipliers[0] = value.get<double>("multipliers.fps_multiplier_1");
    fps_multipliers[1] = value.get<double>("multipliers.fps_multiplier_2");
    fps_multipliers[2] = value.get<double>("multipliers.fps_multiplier_3");
    fps_multipliers[3] = value.get<double>("multipliers.fps_multiplier_4");

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

    const auto reset         = value.get<std::vector<std::string>>("shortcuts.reset"        );
    const auto fullscreen    = value.get<std::vector<std::string>>("shortcuts.fullscreen"   );
    const auto fps_default   = value.get<std::vector<std::string>>("shortcuts.fps_default"  );
    const auto fps_custom_1  = value.get<std::vector<std::string>>("shortcuts.fps_custom_1" );
    const auto fps_custom_2  = value.get<std::vector<std::string>>("shortcuts.fps_custom_2" );
    const auto fps_custom_3  = value.get<std::vector<std::string>>("shortcuts.fps_custom_3" );
    const auto fps_custom_4  = value.get<std::vector<std::string>>("shortcuts.fps_custom_4" );
    const auto fps_unlimited = value.get<std::vector<std::string>>("shortcuts.fps_unlimited");

    shortcuts.keyboard.reset         = keyByName(reset[0]);
    shortcuts.keyboard.fullscreen    = keyByName(fullscreen[0]);
    shortcuts.keyboard.fps_default   = keyByName(fps_default[0]);
    shortcuts.keyboard.fps_custom_1  = keyByName(fps_custom_1[0]);
    shortcuts.keyboard.fps_custom_2  = keyByName(fps_custom_2[0]);
    shortcuts.keyboard.fps_custom_3  = keyByName(fps_custom_3[0]);
    shortcuts.keyboard.fps_custom_4  = keyByName(fps_custom_4[0]);
    shortcuts.keyboard.fps_unlimited = keyByName(fps_unlimited[0]);

    shortcuts.controller.reset         = buttonByName(reset[1]);
    shortcuts.controller.fullscreen    = buttonByName(fullscreen[1]);
    shortcuts.controller.fps_default   = buttonByName(fps_default[1]);
    shortcuts.controller.fps_custom_1  = buttonByName(fps_custom_1[1]);
    shortcuts.controller.fps_custom_2  = buttonByName(fps_custom_2[1]);
    shortcuts.controller.fps_custom_3  = buttonByName(fps_custom_3[1]);
    shortcuts.controller.fps_custom_4  = buttonByName(fps_custom_4[1]);
    shortcuts.controller.fps_unlimited = buttonByName(fps_unlimited[1]);
}

void Config::initDefault()
{
    bios_file = "";
    bios_skip = true;
    save_dir  = "";
    deadzone  = 16000;

    fps_multipliers[0] = 2.0;
    fps_multipliers[1] = 4.0;
    fps_multipliers[2] = 6.0;
    fps_multipliers[3] = 8.0;

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
    controls.controller.up     = BTN_DPAD_UP;
    controls.controller.down   = BTN_DPAD_DOWN;
    controls.controller.left   = BTN_DPAD_LEFT;
    controls.controller.right  = BTN_DPAD_RIGHT;
    controls.controller.start  = BTN_START;
    controls.controller.select = BTN_BACK;
    controls.controller.l      = BTN_LEFTSHOULDER;
    controls.controller.r      = BTN_RIGHTSHOULDER;

    shortcuts.keyboard.reset         = KEY_R;
    shortcuts.keyboard.fullscreen    = KEY_F11;
    shortcuts.keyboard.fps_default   = KEY_1;
    shortcuts.keyboard.fps_custom_1  = KEY_2;
    shortcuts.keyboard.fps_custom_2  = KEY_3;
    shortcuts.keyboard.fps_custom_3  = KEY_4;
    shortcuts.keyboard.fps_custom_4  = KEY_5;
    shortcuts.keyboard.fps_unlimited = KEY_6;

    shortcuts.controller.reset         = BTN_NONE;
    shortcuts.controller.fullscreen    = BTN_NONE;
    shortcuts.controller.fps_default   = BTN_NONE;
    shortcuts.controller.fps_custom_1  = BTN_NONE;
    shortcuts.controller.fps_custom_2  = BTN_NONE;
    shortcuts.controller.fps_custom_3  = BTN_NONE;
    shortcuts.controller.fps_custom_4  = BTN_NONE;
    shortcuts.controller.fps_unlimited = BTN_NONE;
}
