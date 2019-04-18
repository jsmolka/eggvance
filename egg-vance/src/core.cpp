#include "core.h"

#include "common/memory_map.h"

Core::Core()
    : arm(mmu)
{
    ppu.mmu = &mmu;

    reset();
}

void Core::run(const std::string& file)
{
    if (!mmu.loadRom(file))
        return;

    bool running = true;

    SDL_Event event;
    while (running)
    {
        for (int i = 0; i < 12500; ++i)
            arm.step(); 

        ppu.renderFrame();

        u16 keyinput = mmu.readHalf(REG_KEYINPUT);

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                else
                    keyinput |= keyMask(event.key.keysym.sym);
                break;

            case SDL_KEYUP:
                keyinput &= ~keyMask(event.key.keysym.sym);
                break;
            }
        }

        mmu.writeHalf(REG_KEYINPUT, keyinput);
    }
}

void Core::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();

    mmu.writeHalf(REG_KEYINPUT, 0xFF);
}

u16 Core::keyMask(const SDL_Keycode& key)
{
    switch (key)
    {
    case SDLK_u: return 1 << 0;  // Button A
    case SDLK_h: return 1 << 1;  // Button B
    case SDLK_f: return 1 << 2;  // Select
    case SDLK_g: return 1 << 3;  // Start
    case SDLK_d: return 1 << 4;  // Right
    case SDLK_a: return 1 << 5;  // Left
    case SDLK_w: return 1 << 6;  // Up
    case SDLK_s: return 1 << 7;  // Down
    case SDLK_i: return 1 << 8;  // Button R
    case SDLK_q: return 1 << 9;  // Button L
    }
    return 0;
}
