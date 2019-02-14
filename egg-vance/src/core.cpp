#include "core.h"

#include "common/log.h"
#include "common/memory_map.h"

Core::Core()
{
    arm.mmu = &mmu;
    ppu.mmu = &mmu;

    arm.reset();
    mmu.reset();
    ppu.reset();
}

void Core::run(const std::string& file)
{
    if (!mmu.loadRom(file))
        return;

    while (arm.running)
    {
        for (int i = 0; i < 4096; ++i)
            arm.step(); 

        ppu.renderText();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                arm.running = false;
        }
    }
}
