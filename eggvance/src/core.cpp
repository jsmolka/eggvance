#include "core.h"

#include <SDL2/SDL_events.h>

#include "mmu/interrupt.h"

Core::Core()
    : arm(mmu)
    , ppu(mmu)
    , input(mmu)
{
    Interrupt::init(&mmu);
}

void Core::run(const std::string& file)
{
    if (!mmu.readFile(file))
        return;

    if (!mmu.readBios("bios.bin"))
        return;

    while (true)
    {
        u32 ticks = SDL_GetTicks();

        frame();

        u32 delta = SDL_GetTicks() - ticks;
        if (delta < 16)
            SDL_Delay(16 - delta);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_F11 && event.key.state == SDL_PRESSED)
                    ppu.backend.fullscreen();
                else
                    input.handleKeyEvent(event.key);
                break;

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                input.updateController(event.cdevice.which);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                input.handleControllerButtonEvent(event.cbutton);
                break;

            case SDL_CONTROLLERAXISMOTION:
                input.handleControllerAxisEvent(event.caxis);
                break;
            }
        }
    } 
}

void Core::frame()
{
    for (int line = 0; line < 160; ++line)
    {
        emulate(960);

        ppu.scanline();
        ppu.hblank();
        
        emulate(272);

        ppu.next();
    }

    ppu.vblank();
    for (int line = 0; line < 68; ++line)
    {
        emulate(960 + 272);
        ppu.next();
    }

    ppu.present();
}

void Core::emulate(int cycles)
{
    static int remaining = 0;
    remaining += cycles;

    while (remaining > 0)
    {
        if (mmu.dmas_active.empty())
        {
            if (Interrupt::requested())
            {
                arm.interrupt();
            }
            if (mmu.halt)
            {
                // Todo: Emulate until first interrupt
                emulateTimers(remaining);
                remaining = 0;
                break;
            }
            else
            {
                cycles = arm.step();
                remaining -= cycles;
                emulateTimers(cycles);
            }
        }
        else
        {
            if (mmu.dmas_active.front()->emulate(remaining))
            {
                mmu.dmas_active.pop_front();
            }
        }
    }
}

void Core::emulateTimers(int cycles)
{
    for (Timer& timer : mmu.timers)
    {
        timer.emulate(cycles);
    }
}
