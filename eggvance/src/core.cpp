#include "core.h"

#include <filesystem>
#include <sstream>
#include <fmt/format.h>

#include "mmu/interrupt.h"
#include "icon.h"
#include "microclock.h"

namespace micro = micro_clock;
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
        mmu.gamepak = std::move(gamepak);
    }
    else
    {
        if (!dropAwait())
            return;
    }

    int time_emulator = 0;
    int time_hardware = 16740;

    u32 fps_frame = 0;
    u32 fps_begin = SDL_GetTicks();

    while (true)
    {
        u64 begin = micro::now();

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

            case SDL_DROPFILE:
                if (!dropEvent(event.drop))
                    return;
                break;
            }
        }

        frame();

        if (limited)
        {
            time_emulator += static_cast<u32>(micro::now() - begin);
            if (time_emulator < time_hardware)
            {
                u64 begin = micro::now();
                micro::sleep(time_hardware - time_emulator);
                time_emulator += static_cast<u32>(micro::now() - begin);
            }
            time_emulator -= time_hardware;
        }
        
        fps_frame++;
        u32 delta = SDL_GetTicks() - fps_begin;
        if (delta >= 1000)
        {

            std::string fps = fmt::format(" - {:.1f} fps", (1000.f / static_cast<double>(delta) * static_cast<double>(fps_frame)));
            SDL_SetWindowTitle(ppu.backend.window, (window_title + fps).c_str());

            fps_begin = SDL_GetTicks();
            fps_frame = 0;
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
    SDL_SetRenderDrawColor(renderer, 38, 40, 43, 1);
    SDL_RenderFillRect(renderer, &rect);

    int scale = 9;
    rect.w = scale;
    rect.h = scale;
    for (const auto& pixel : icon)
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
        if (mmu.gamepak && mmu.gamepak->file == file)
        {
            reset();
            return true;
        }

        auto gamepak = std::make_unique<GamePak>(file);
        if (!gamepak->valid)
            return false;

        reset();
        setWindowTitle(*gamepak);
        mmu.gamepak = std::move(gamepak);
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
    window_title = stream.str();
    SDL_SetWindowTitle(ppu.backend.window, window_title.c_str());
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
    for (auto& timer : mmu.timers)
    {
        timer.emulate(cycles);
    }
}
