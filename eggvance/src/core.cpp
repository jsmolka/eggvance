#include "core.h"

#include "mmu/map.h"

Core::Core()
    : arm(mmu)
    , ppu(mmu)
{
    reset();
}

void Core::run(const std::string& file)
{
    if (!mmu.readFile(file, MAP_GAMEPAK_0))
        return;

    if (!mmu.readFile("bios.bin", MAP_BIOS))
        return;

    bool running = true;
    while (running)
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
                running = false;
                break;

            case SDL_KEYDOWN:
                keyEvent(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                keyEvent(event.key.keysym.sym, false);
                break;
            }
        }
    }   
}

void Core::reset()
{
    mmu.reset();
    arm.reset();
    ppu.reset();
    
    mmu.keyinput = 0x3FF;
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
        if (mmu.int_master && (mmu.int_enabled & mmu.int_request))
        {
            arm.interrupt();
        }
        if (mmu.halt)
        {
            // Todo: inaccurate, should emulate until first interrupt
            emulateTimers(remaining);
            remaining = 0;
            return;
        }
        cycles = arm.step();
        remaining -= cycles;
        emulateTimers(cycles);
    }
}

void Core::emulateTimers(int cycles)
{
    static InterruptFlag flags[4] = {
        IF_TIMER0_OVERFLOW,
        IF_TIMER1_OVERFLOW,
        IF_TIMER2_OVERFLOW,
        IF_TIMER3_OVERFLOW
    };

    while (cycles-- > 0)
    {
        for (int x = 0; x < 4; ++x)
        {
            mmu.timer[x].step();

            if (mmu.timer[x].requestInterrupt()) 
            {
                mmu.requestInterrupt(flags[x]);
            }
        }
    }
}

void Core::keyEvent(SDL_Keycode key, bool pressed)
{
    int state = pressed ? 0 : 1;

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

    if (mmu.keycnt.irq)
    {
        int input = mmu.keyinput & 0x3FF;
        int control = mmu.keycnt & 0x3FF;

        bool interrupt = mmu.keycnt.logic
            ? (input & control)
            : (input | control);

        if (interrupt)
        {
            mmu.requestInterrupt(IF_KEYPAD);
        }
    }
}
