#include "core.h"

#include <filesystem>

#include "mmu/interrupt.h"
#include "icon.h"

namespace fs = std::filesystem;

Core::Core(std::shared_ptr<BIOS> bios)
    : mmu(bios)
    , arm(mmu)
    , ppu(mmu)
    , input(mmu)
{
    Interrupt::init(&mmu);

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

void Core::run(std::shared_ptr<GamePak> gamepak)
{
    if (gamepak && gamepak->valid)
    {
        mmu.setGamePak(gamepak);
    }
    else
    {
        if (!dropAwait())
            return;
    }

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

            case SDL_DROPFILE:
                dropEvent(event.drop);
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
}

void Core::drawIcon()
{
    SDL_Rect rect = { 0, 0, WIDTH, HEIGHT };
    SDL_Renderer* renderer = ppu.backend.renderer;
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 1);
    SDL_RenderFillRect(renderer, &rect);

    int scale = 9;
    rect.w = scale;
    rect.h = scale;
    for (const IconPixel& pixel : icon)
    {
        rect.x = scale * pixel.x + 48;
        rect.y = scale * pixel.y + 8;
        SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, 1);
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool Core::dropAwait()
{
    drawIcon();

    while (true)
    {
        SDL_Delay(16);
        SDL_RenderPresent(ppu.backend.renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return false;

            case SDL_DROPFILE:
                if (dropEvent(event.drop))
                    return true;
                break;
            }
        }
    }
}

bool Core::dropEvent(const SDL_DropEvent& event)
{
    std::string file = event.file;
    SDL_free(event.file);

    if (fs::is_regular_file(file))
    {
        auto gamepak = std::make_shared<GamePak>(file);
        if (!gamepak->valid)
            return false;

        reset();
        mmu.setGamePak(gamepak);
        SDL_RaiseWindow(ppu.backend.window);
        return true;
    }
    return false;
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
            if (mmu.dmas_active.back()->emulate(remaining))
            {
                mmu.dmas_active.pop_back();
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
