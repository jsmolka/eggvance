#pragma once

#include <optional>
#include <string>

#include "gamepak/gpio.h"
#include "gamepak/save.h"

struct Overwrite
{
    Save::Type save_type;
    Gpio::Type gpio_type;
};

std::optional<Overwrite> findOverwrite(const std::string& game_code);
