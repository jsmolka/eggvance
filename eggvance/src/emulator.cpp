#include "emulator.h"

#include <SDL2/SDL.h>

#include "common/config.h"
#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"

void Emulator::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();
}

bool Emulator::init(const Args& args)
{
    config.init(args.dir);

    if (!mmu.bios.init())
        return false;

    if (!ppu.backend.init())
        return false;

    if (!args.rom.empty())
        mmu.gamepak.load(args.rom);

    return true;
}

void Emulator::run()
{
    reset();

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return;
            }
        }

        frame();
    }
}

void Emulator::frame()
{
    for (int line = 0; line < 160; ++line)
    {
        arm.run(960);
        ppu.scanline();
        ppu.hblank();
        arm.run(272);
        ppu.next();
    }

    ppu.vblank();
    for (int line = 0; line < 68; ++line)
    {
        arm.run(960 + 272);
        ppu.next();
    }
    ppu.present();
}
