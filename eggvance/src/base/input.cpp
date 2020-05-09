#include "input.h"

#include <algorithm>
#include <cctype>
#include <string_view>

Key keyByName(std::string name)
{
    static constexpr std::pair<std::string_view, Key> key_map[] =
    {
        { "A"  , Key::A   },
        { "B"  , Key::B   },
        { "C"  , Key::C   },
        { "D"  , Key::D   },
        { "E"  , Key::E   },
        { "F"  , Key::F   },
        { "G"  , Key::G   },
        { "H"  , Key::H   },
        { "I"  , Key::I   },
        { "J"  , Key::J   },
        { "K"  , Key::K   },
        { "L"  , Key::L   },
        { "M"  , Key::M   },
        { "N"  , Key::N   },
        { "O"  , Key::O   },
        { "P"  , Key::P   },
        { "Q"  , Key::Q   },
        { "R"  , Key::R   },
        { "S"  , Key::S   },
        { "T"  , Key::T   },
        { "U"  , Key::U   },
        { "V"  , Key::V   },
        { "W"  , Key::W   },
        { "X"  , Key::X   },
        { "Y"  , Key::Y   },
        { "Z"  , Key::Z   },
        { "0"  , Key::N0  },
        { "1"  , Key::N1  },
        { "2"  , Key::N2  },
        { "3"  , Key::N3  },
        { "4"  , Key::N4  },
        { "5"  , Key::N5  },
        { "6"  , Key::N6  },
        { "7"  , Key::N7  },
        { "8"  , Key::N8  },
        { "9"  , Key::N9  },
        { "F1" , Key::F1  },
        { "F2" , Key::F2  },
        { "F3" , Key::F3  },
        { "F4" , Key::F4  },
        { "F5" , Key::F5  },
        { "F6" , Key::F6  },
        { "F7" , Key::F7  },
        { "F8" , Key::F8  },
        { "F9" , Key::F9  },
        { "F10", Key::F10 },
        { "F11", Key::F11 },
        { "F12", Key::F12 }
    };

    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c) { return std::toupper(c); });

    for (const auto& [key, value] : key_map)
    {
        if (key == name)
            return value;
    }
    return Key::None;
}

Button buttonByName(std::string name)
{
    static constexpr std::pair<std::string_view, Button> button_map[] =
    {
        { "A"     , Button::A      },
        { "B"     , Button::B      },
        { "X"     , Button::X      },
        { "Y"     , Button::Y      },
        { "BACK"  , Button::Back   },
        { "GUIDE" , Button::Guide  },
        { "START" , Button::Start  },
        { "LSTICK", Button::LStick },
        { "RSTICK", Button::RStick },
        { "L"     , Button::L      },
        { "R"     , Button::R      },
        { "UP"    , Button::Up     },
        { "DOWN"  , Button::Down   },
        { "LEFT"  , Button::Left   },
        { "RIGHT" , Button::Right  }
    };

    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c) { return std::toupper(c); });

    for (const auto& [key, value] : button_map)
    {
        if (key == name)
            return value;
    }
    return Button::None;
}
