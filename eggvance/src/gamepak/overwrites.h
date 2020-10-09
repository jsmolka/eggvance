#pragma once

#include <optional>
#include <string>

#include "gamepak/gpio.h"
#include "gamepak/save.h"

class Overwrite
{
public:
    static std::optional<Overwrite> find(const std::string& game_code);

    Save::Type save_type;
    Gpio::Type gpio_type;
};

