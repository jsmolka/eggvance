#include "config.h"

#include <filesystem>

namespace fs = std::filesystem;

Config config;

void Config::init(const std::string& dir)
{
    deadzone = 16000;

    bios_file = fs::path(dir).append("bios.bin").string();
    bios_skip = true;

    keyboard_map.insert({
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
    
    controller_map.insert({
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
}