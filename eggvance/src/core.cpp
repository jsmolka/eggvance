#include "core.h"

#include <filesystem>

#include "common/format.h"
#include "mmu/interrupt.h"
#include "icon.h"

namespace fs = std::filesystem;

Core::Core(std::shared_ptr<BIOS> bios)
    : remaining(0)
    , mmu(bios)
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
        updateTitle(gamepak);
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

            case SDL_KEYUP:
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_F11 && event.key.state == SDL_PRESSED)
                    ppu.backend.fullscreen();
                else
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
        auto gamepak = std::make_shared<GamePak>(file);
        if (!gamepak->valid)
            return false;

        reset();
        updateTitle(gamepak);
        mmu.setGamePak(gamepak);
        SDL_RaiseWindow(ppu.backend.window);
        return true;
    }
    return false;
}

void Core::updateTitle(std::shared_ptr<GamePak> gamepak)
{
    std::string save;
    switch (gamepak->save->type)
    {
    case Save::Type::SRAM:
        save = "SRAM";
        break;

    case Save::Type::FLASH64:
        save = "FLASH64";
        break;

    case Save::Type::FLASH128:
        save = "FLASH128";
        break;

    case Save::Type::EEPROM:
        save = "EEPROM";
        break;

    case Save::Type::NONE:
        save = "NONE";
        break;
    }
    std::string title = fmt::format("eggvance - {} - {}", gamepak->header.title.c_str(), save.c_str());

    SDL_SetWindowTitle(ppu.backend.window, title.c_str());
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
