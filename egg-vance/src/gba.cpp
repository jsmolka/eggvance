#include "gba.h"

#include <iostream>

#include "memory_map.h"

GBA::GBA()
{
    mmu.lcd_stat = &lcd.stat;

    arm.mmu = &mmu;
    lcd.mmu = &mmu;

    reset();
}

void GBA::run(const std::string& filepath)
{
    if (!mmu.loadRom(filepath))
    {
        std::cout << __FUNCTION__ << " - Could not load ROM " << filepath << "\n";
        return;
    }

    while (arm.running)
    {
        for (int i = 0; i < 4096; ++i)
            arm.step(); 

        lcd.drawBg0();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                arm.running = false;
        }
    }
}

void GBA::reset()
{
    arm.reset();
    mmu.reset();
    lcd.reset();
}
