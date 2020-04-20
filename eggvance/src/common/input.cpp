#include "input.h"

#include <algorithm>
#include <cctype>
#include <string_view>

static constexpr std::pair<std::string_view, Key> key_map[] =
{
    { "A"        , KEY_A         },
    { "B"        , KEY_B         },
    { "C"        , KEY_C         },
    { "D"        , KEY_D         },
    { "E"        , KEY_E         },
    { "F"        , KEY_F         },
    { "G"        , KEY_G         },
    { "H"        , KEY_H         },
    { "I"        , KEY_I         },
    { "J"        , KEY_J         },
    { "K"        , KEY_K         },
    { "L"        , KEY_L         },
    { "M"        , KEY_M         },
    { "N"        , KEY_N         },
    { "O"        , KEY_O         },
    { "P"        , KEY_P         },
    { "Q"        , KEY_Q         },
    { "R"        , KEY_R         },
    { "S"        , KEY_S         },
    { "T"        , KEY_T         },
    { "U"        , KEY_U         },
    { "V"        , KEY_V         },
    { "W"        , KEY_W         },
    { "X"        , KEY_X         },
    { "Y"        , KEY_Y         },
    { "Z"        , KEY_Z         },
    { "0"        , KEY_0         },
    { "1"        , KEY_1         },
    { "2"        , KEY_2         },
    { "3"        , KEY_3         },
    { "4"        , KEY_4         },
    { "5"        , KEY_5         },
    { "6"        , KEY_6         },
    { "7"        , KEY_7         },
    { "8"        , KEY_8         },
    { "9"        , KEY_9         },
    { "F1"       , KEY_F1        },
    { "F2"       , KEY_F2        },
    { "F3"       , KEY_F3        },
    { "F4"       , KEY_F4        },
    { "F5"       , KEY_F5        },
    { "F6"       , KEY_F6        },
    { "F7"       , KEY_F7        },
    { "F8"       , KEY_F8        },
    { "F9"       , KEY_F9        },
    { "F10"      , KEY_F10       },
    { "F11"      , KEY_F11       },
    { "F12"      , KEY_F12       },
    { "UP"       , KEY_UP        },
    { "DOWN"     , KEY_DOWN      },
    { "LEFT"     , KEY_LEFT      },
    { "RIGHT"    , KEY_RIGHT     },
    { "RETURN"   , KEY_RETURN    },
    { "ESCAPE"   , KEY_ESCAPE    },
    { "BACKSPACE", KEY_BACKSPACE },
    { "TAB"      , KEY_TAB       },
    { "SPACE"    , KEY_SPACE     },
    { "CAPSLOCK" , KEY_CAPSLOCK  }
};

static constexpr std::pair<std::string_view, Button> button_map[] = 
{
    { "A"            , BTN_A             },
    { "B"            , BTN_B             },
    { "X"            , BTN_X             },
    { "Y"            , BTN_Y             },
    { "BACK"         , BTN_BACK          },
    { "GUIDE"        , BTN_GUIDE         },
    { "START"        , BTN_START         },
    { "LEFTSTICK"    , BTN_LEFTSTICK     },
    { "RIGHTSTICK"   , BTN_RIGHTSTICK    },
    { "LEFTSHOULDER" , BTN_LEFTSHOULDER  },
    { "RIGHTSHOULDER", BTN_RIGHTSHOULDER },
    { "DPUP"         , BTN_DPAD_UP       },
    { "DPDOWN"       , BTN_DPAD_DOWN     },
    { "DPLEFT"       , BTN_DPAD_LEFT     },
    { "DPRIGHT"      , BTN_DPAD_RIGHT    }
};

static std::string upper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::toupper(c); });

    return str;
}

Key keyByName(std::string name)
{
    name = upper(name);
    for (const auto& [key, value] : key_map)
    {
        if (key == name)
            return value;
    }
    return KEY_NONE;
}

Button buttonByName(std::string name)
{
    name = upper(name);
    for (const auto& [key, value] : button_map)
    {
        if (key == name)
            return value;
    }
    return BTN_NONE;
}
