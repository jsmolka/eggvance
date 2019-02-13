#include "core.h"

#include "common.h"
#include "memory_map.h"

Core::Core()
{
    mmu.lcd_stat = &lcd.stat;

    arm.mmu = &mmu;
    lcd.mmu = &mmu;

    reset();
}

void Core::run(const std::string& file)
{
    if (!mmu.loadRom(file))
    {
        fcout() << "Could not load ROM " << file;
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

void Core::reset()
{
    arm.reset();
    mmu.reset();
    lcd.reset();
}
