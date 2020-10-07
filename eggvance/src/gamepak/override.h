#pragma once

#include <optional>
#include <string>

#include "gamepak/gpio.h"
#include "gamepak/save.h"

struct Override
{
    Save::Type save_type;
    Gpio::Type gpio_type;
};

std::optional<Override> findOverride(const std::string& game_code);
