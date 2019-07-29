#include "core.h"

#include "mmu/map.h"
#include "mmu/interrupt.h"

Core::Core()
    : arm(mmu)
    , ppu(mmu)
{
    Interrupt::mmu = &mmu;

    reset();
}

void Core::run(const std::string& file)
{
    if (!mmu.readFile(file))
        return;

    if (!mmu.readBios("bios.bin"))
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
    
    mmu.mmio<u16>(REG_KEYINPUT) = 0x3FF;
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
}

void Core::emulateTimers(int cycles)
{
    for (Timer& timer : mmu.timer)
        timer.emulate(cycles);
}

void Core::keyEvent(SDL_Keycode key, bool pressed)
{
    int state = !pressed;
    int shift = 0;

    switch (key)
    {
    case SDLK_u: shift = 0; break; // A
    case SDLK_h: shift = 1; break; // B
    case SDLK_f: shift = 2; break; // Select
    case SDLK_g: shift = 3; break; // Start
    case SDLK_d: shift = 4; break; // Right
    case SDLK_a: shift = 5; break; // Left
    case SDLK_w: shift = 6; break; // Up
    case SDLK_s: shift = 7; break; // Down
    case SDLK_i: shift = 8; break; // R
    case SDLK_q: shift = 9; break; // L
    }

    u16& keyinput = mmu.mmio<u16>(REG_KEYINPUT);
    
    keyinput &= ~(1 << shift);
    keyinput |= (state << shift);

    if (mmu.keycnt.irq)
    {
        bool interrupt = mmu.keycnt.logic
            ? (keyinput & mmu.keycnt.mask)
            : (keyinput | mmu.keycnt.mask);

        if (interrupt)
        {
            Interrupt::request(IF_KEYPAD);
        }
    }
}
