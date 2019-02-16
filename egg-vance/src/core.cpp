#include "core.h"

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

    SDL_Event event;
    while (arm.running)
    {
        // Todo: execute instruction until one frame can be drawn
        for (int i = 0; i < 4096; ++i)
            arm.step(); 

        ppu.renderFrame();

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                arm.running = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    arm.running = false;
                break;
            }
        }
    }
}
