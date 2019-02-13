#pragma once

#include <string>

#include "arm7.h"
#include "lcd.h"
#include "mmu.h"

class Core
{
public:
    Core();

    void run(const std::string &file);

private:
    void reset();

    Arm7 arm;
    Mmu mmu;
    Lcd lcd;
};

