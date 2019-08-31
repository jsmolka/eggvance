#include "core.h"

#include <filesystem>
#include <sstream>

#include "mmu/interrupt.h"
#include "icon.h"

namespace fs = std::filesystem;

Core::Core(std::unique_ptr<BIOS> bios)
    : mmu(std::move(bios))
    , arm(mmu)
    , ppu(mmu)
    , input(mmu.mmio)
    , remaining(0)
    , limited(true)
{
    Interrupt::init(&mmu.mmio);
    mmu.bios->setPC(&arm.regs.pc);

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

void Core::run(std::unique_ptr<GamePak> gamepak)
{
    if (gamepak && gamepak->valid)
    {
        setWindowTitle(*gamepak);
        mmu.setGamePak(std::move(gamepak));
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
        if (limited && delta < 16)
            SDL_Delay(16 - delta);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return;

            case SDL_KEYUP:
            case SDL_KEYDOWN:
                coreKeyEvent(event.key);
                input.keyEvent(event.key);
                break;

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                input.controllerDeviceEvent(event.cdevice);
                break;

            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN:
                input.controllerButtonEvent(event.cbutton);
                break;

            case SDL_CONTROLLERAXISMOTION:
                input.controllerAxisEvent(event.caxis);
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
    SDL_SetRenderDrawColor(renderer, 33, 35, 36, 1);
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

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                input.controllerDeviceEvent(event.cdevice);
                break;

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
        auto gamepak = std::make_unique<GamePak>(file);
        if (!gamepak->valid)
            return false;

        setWindowTitle(*gamepak);
        mmu.setGamePak(std::move(gamepak));
        SDL_RaiseWindow(ppu.backend.window);
        return true;
    }
    return false;
}

void Core::coreKeyEvent(const SDL_KeyboardEvent& event)
{
    if (event.state == SDL_RELEASED)
        return;

    switch (event.keysym.sym)
    {
    case SDLK_LSHIFT:
        limited ^= true;
        break;

    case SDLK_F11:
        ppu.backend.fullscreen();
        break;

    case SDLK_r:
        reset();
        break;
    }
}

void Core::setWindowTitle(const GamePak& gamepak)
{
    std::stringstream stream;
    stream << "eggvance";
    if (!gamepak.header.title.empty())
    {
        stream << " - ";
        stream << gamepak.header.title;
    }
    switch (gamepak.save->type)
    {
    case Save::Type::SRAM:
        stream << " - SRAM";
        break;

    case Save::Type::FLASH64:
        stream << " - FLASH64";
        break;

    case Save::Type::FLASH128:
        stream << " - FLASH128";
        break;

    case Save::Type::EEPROM:
        stream << " - EEPROM";
        break;
    }
    SDL_SetWindowTitle(ppu.backend.window, stream.str().c_str());
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
    // Todo: maybe remove remaining from hot loop?
    remaining += cycles;

    while (remaining > 0)
    {
        if (mmu.dmas_active.empty())
        {
            if (Interrupt::requested())
            {
                arm.interrupt();
            }
            if (mmu.mmio.halt)
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
            cycles = remaining;
            if (mmu.dmas_active.back()->emulate(remaining))
            {
                mmu.dmas_active.pop_back();
            }
            emulateTimers(cycles - remaining);
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
