#pragma once

#include <string>

#include "arm7.h"
#include "lcd.h"
#include "mmu.h"

class GBA
{
public:
    GBA();

    void run(const std::string &filepath);

private:
    void reset();

    ARM7 arm;
    MMU mmu;
    LCD lcd;
};

