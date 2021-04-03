#pragma once

#include <optional>
#include <string>

#include "gpio.h"
#include "save.h"

class Overwrite
{
public:
    static std::optional<Overwrite> find(const std::string& game_code);

    Save::Type save_type = Save::Type::Detect;
    Gpio::Type gpio_type = Gpio::Type::Detect;
    bool mirror = false;
};

