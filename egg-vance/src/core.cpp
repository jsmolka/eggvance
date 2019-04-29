#include "core.h"

#include "common/memory_map.h"

Core::Core()
    : arm(mmu)
    , ppu(mmu)
{
    reset();
}

void Core::run(const std::string& file)
{
    if (!mmu.loadRom(file))
        return;

    bool running = true;
    while (running)
    {
        frame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                processKeyEvent(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                processKeyEvent(event.key.keysym.sym, false);
                break;
            }
        }
    }
}

void Core::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();

    // Set keys to not pressed
    mmu.keyinput.data = 0xFF;
}

void Core::frame()
{
    // Visible lines
    for (int line = 0; line < 160; ++line)
    {
        ppu.scanline();
        runCycles(960);

        // H-Blank
        ppu.hblank();
        runCycles(272);

        ppu.next();
    }

    // Invisible lines
    ppu.vblank();
    for (int line = 0; line < 68; ++line)
    {
        runCycles(960 + 272);
        ppu.next();
    }

    ppu.update();
}

void Core::runCycles(int cycles)
{
    static int cycles_left = 0;

    cycles_left += cycles;

    while (cycles_left >= 0)
    {
        cycles_left -= arm.step();
    }
}

void Core::processKeyEvent(SDL_Keycode key, bool down)
{
    int state = down ? 0 : 1;

    switch (key)
    {
    case SDLK_u: mmu.keyinput.a      = state; break;
    case SDLK_h: mmu.keyinput.b      = state; break;
    case SDLK_f: mmu.keyinput.select = state; break;
    case SDLK_g: mmu.keyinput.start  = state; break;
    case SDLK_d: mmu.keyinput.right  = state; break;
    case SDLK_a: mmu.keyinput.left   = state; break;
    case SDLK_w: mmu.keyinput.up     = state; break;
    case SDLK_s: mmu.keyinput.down   = state; break;
    case SDLK_i: mmu.keyinput.r      = state; break;
    case SDLK_q: mmu.keyinput.l      = state; break;
    }
}
