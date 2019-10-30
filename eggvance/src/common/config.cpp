#include "config.h"

#include <filesystem>
#include <toml/toml.h>

#include "common/fileutil.h"

namespace fs = std::filesystem;

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
    auto stream = std::ifstream(fileutil::toAbsolute("eggvance.toml"));
    auto result = toml::parse(stream);
    if (!result.valid())
        throw std::exception();

    auto& value = result.value;

    bios_file = value.get<std::string>("general.bios_file");
    save_dir  = value.get<std::string>("general.save_dir");
    bios_skip = value.get<bool>("general.bios_skip");
    deadzone  = value.get<int>("general.deadzone");

    if (fs::path(bios_file).is_relative())
    {
        bios_file = fileutil::toAbsolute(bios_file);
    }
    if (!save_dir.empty())
    {
        if (fs::path(save_dir).is_relative())
            save_dir = fileutil::toAbsolute(save_dir);
        if (!fs::is_directory(save_dir) || !fs::exists(save_dir))
            fs::create_directories(save_dir);
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

    this->controls.keyboard.insert({
        { SDL_GetKeyFromName(a[0].c_str())     , Keypad::Button::A     },
        { SDL_GetKeyFromName(b[0].c_str())     , Keypad::Button::B     },
        { SDL_GetKeyFromName(up[0].c_str())    , Keypad::Button::Up    },
        { SDL_GetKeyFromName(down[0].c_str())  , Keypad::Button::Down  },
        { SDL_GetKeyFromName(left[0].c_str())  , Keypad::Button::Left  },
        { SDL_GetKeyFromName(right[0].c_str()) , Keypad::Button::Right },
        { SDL_GetKeyFromName(start[0].c_str()) , Keypad::Button::Start },
        { SDL_GetKeyFromName(select[0].c_str()), Keypad::Button::Select},
        { SDL_GetKeyFromName(l[0].c_str())     , Keypad::Button::R     },
        { SDL_GetKeyFromName(r[0].c_str())     , Keypad::Button::L     } 
    });
    
    this->controls.controller.insert({
        { SDL_GameControllerGetButtonFromString(a[1].c_str())     , Keypad::Button::A     },
        { SDL_GameControllerGetButtonFromString(b[1].c_str())     , Keypad::Button::B     },
        { SDL_GameControllerGetButtonFromString(up[1].c_str())    , Keypad::Button::Up    },
        { SDL_GameControllerGetButtonFromString(down[1].c_str())  , Keypad::Button::Down  },
        { SDL_GameControllerGetButtonFromString(left[1].c_str())  , Keypad::Button::Left  },
        { SDL_GameControllerGetButtonFromString(right[1].c_str()) , Keypad::Button::Right },
        { SDL_GameControllerGetButtonFromString(start[1].c_str()) , Keypad::Button::Start },
        { SDL_GameControllerGetButtonFromString(select[1].c_str()), Keypad::Button::Select},
        { SDL_GameControllerGetButtonFromString(l[1].c_str())     , Keypad::Button::R     },
        { SDL_GameControllerGetButtonFromString(r[1].c_str())     , Keypad::Button::L     } 
    });

    const auto reset         = value.get<std::vector<std::string>>("shortcuts.reset"        );
    const auto fullscreen    = value.get<std::vector<std::string>>("shortcuts.fullscreen"   );
    const auto fps_default   = value.get<std::vector<std::string>>("shortcuts.fps_default"  );
    const auto fps_option_1  = value.get<std::vector<std::string>>("shortcuts.fps_option_1" );
    const auto fps_option_2  = value.get<std::vector<std::string>>("shortcuts.fps_option_2" );
    const auto fps_option_3  = value.get<std::vector<std::string>>("shortcuts.fps_option_3" );
    const auto fps_option_4  = value.get<std::vector<std::string>>("shortcuts.fps_option_4" );
    const auto fps_unlimited = value.get<std::vector<std::string>>("shortcuts.fps_unlimited");

    this->shortcuts.keyboard.insert({
        { SDL_GetKeyFromName(reset[0].c_str())        , Shortcut::Reset         },
        { SDL_GetKeyFromName(fullscreen[0].c_str())   , Shortcut::Fullscreen    },
        { SDL_GetKeyFromName(fps_default[0].c_str())  , Shortcut::SpeedDefault  },
        { SDL_GetKeyFromName(fps_option_1[0].c_str()) , Shortcut::SpeedOption1  },
        { SDL_GetKeyFromName(fps_option_2[0].c_str()) , Shortcut::SpeedOption2  },
        { SDL_GetKeyFromName(fps_option_3[0].c_str()) , Shortcut::SpeedOption3  },
        { SDL_GetKeyFromName(fps_option_4[0].c_str()) , Shortcut::SpeedOption4  },
        { SDL_GetKeyFromName(fps_unlimited[0].c_str()), Shortcut::SpeedUnlimited}
    });    
    
    this->shortcuts.controller.insert({
        { SDL_GameControllerGetButtonFromString(reset[1].c_str())        , Shortcut::Reset         },
        { SDL_GameControllerGetButtonFromString(fullscreen[1].c_str())   , Shortcut::Fullscreen    },
        { SDL_GameControllerGetButtonFromString(fps_default[1].c_str())  , Shortcut::SpeedDefault  },
        { SDL_GameControllerGetButtonFromString(fps_option_1[1].c_str()) , Shortcut::SpeedOption1  },
        { SDL_GameControllerGetButtonFromString(fps_option_2[1].c_str()) , Shortcut::SpeedOption2  },
        { SDL_GameControllerGetButtonFromString(fps_option_3[1].c_str()) , Shortcut::SpeedOption3  },
        { SDL_GameControllerGetButtonFromString(fps_option_4[1].c_str()) , Shortcut::SpeedOption4  },
        { SDL_GameControllerGetButtonFromString(fps_unlimited[1].c_str()), Shortcut::SpeedUnlimited}
    });
}

void Config::initDefault()
{
    bios_file = fileutil::toAbsolute("bios.bin");
    bios_skip = true;
    save_dir  = "";
    deadzone  = 16000;

    fps_multipliers[0] = 2.0;
    fps_multipliers[1] = 4.0;
    fps_multipliers[2] = 6.0;
    fps_multipliers[3] = 8.0;

    controls.keyboard.clear();
    controls.keyboard.insert({
        { SDLK_u, Keypad::Button::A      },
        { SDLK_h, Keypad::Button::B      },
        { SDLK_f, Keypad::Button::Select },
        { SDLK_g, Keypad::Button::Start  },
        { SDLK_d, Keypad::Button::Right  },
        { SDLK_a, Keypad::Button::Left   },
        { SDLK_w, Keypad::Button::Up     },
        { SDLK_s, Keypad::Button::Down   },
        { SDLK_i, Keypad::Button::R      },
        { SDLK_q, Keypad::Button::L      } 
    });

    controls.controller.clear();
    controls.controller.insert({
        { SDL_CONTROLLER_BUTTON_B,             Keypad::Button::A      },
        { SDL_CONTROLLER_BUTTON_A,             Keypad::Button::B      },
        { SDL_CONTROLLER_BUTTON_BACK,          Keypad::Button::Select },
        { SDL_CONTROLLER_BUTTON_START,         Keypad::Button::Start  },
        { SDL_CONTROLLER_BUTTON_DPAD_RIGHT,    Keypad::Button::Right  },
        { SDL_CONTROLLER_BUTTON_DPAD_LEFT,     Keypad::Button::Left   },
        { SDL_CONTROLLER_BUTTON_DPAD_UP,       Keypad::Button::Up     },
        { SDL_CONTROLLER_BUTTON_DPAD_DOWN,     Keypad::Button::Down   },
        { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, Keypad::Button::R      },
        { SDL_CONTROLLER_BUTTON_LEFTSHOULDER,  Keypad::Button::L      }
    });

    shortcuts.keyboard.clear();
    shortcuts.keyboard.insert({
        { SDLK_r  , Shortcut::Reset          },
        { SDLK_F11, Shortcut::Fullscreen     },
        { SDLK_1  , Shortcut::SpeedDefault   },
        { SDLK_2  , Shortcut::SpeedOption1   },
        { SDLK_3  , Shortcut::SpeedOption2   },
        { SDLK_4  , Shortcut::SpeedOption3   },
        { SDLK_5  , Shortcut::SpeedOption4   },
        { SDLK_6  , Shortcut::SpeedUnlimited },
    });

    shortcuts.controller.clear();
}
