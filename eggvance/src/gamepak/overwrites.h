#pragma once

#include <optional>
#include <string>

#include "gpio.h"
#include "save.h"

class Overwrite
{
public:
    static std::optional<Overwrite> find(const std::string& game_code);

    Save::Type save;
    Gpio::Type gpio;
};

