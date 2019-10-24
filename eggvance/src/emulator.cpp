#include "emulator.h"

#include <filesystem>
#include <sstream>
#include <fmt/format.h>
#include <SDL2/SDL.h>

#include "common/config.h"
#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "sys/keypad.h"
#include "framelimiter.h"
#include "icon.h"

namespace fs = std::filesystem;

Emulator::Emulator()
{
    limiter.setFPS(59.737);

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

void Emulator::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();
    keypad.reset();
}

bool Emulator::init(const Args& args)
{
    config.init(args.dir);

    if (!mmu.bios.init())
        return false;

    if (!keypad.init())
        return false;

    if (!ppu.backend.init())
        return false;

    if (!args.rom.empty())
        mmu.gamepak.load(args.rom);

    return true;
}

void Emulator::run()
{
    if (mmu.gamepak.size() == 0)
    {
        if (!dropAwait())
            return;
    }
    else
    {
        updateWindowTitle();
        reset();
    }

    u32 fps_frame = 0;
    u32 fps_begin = SDL_GetTicks();

    while (true)
    {
        limiter.frameBegin();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return;

            case SDL_KEYDOWN:
                keyboardEvent(event.key);
            case SDL_KEYUP:
                keypad.keyboardEvent(event.key);
                break;

            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN:
                keypad.controllerButtonEvent(event.cbutton);
                break;

            case SDL_CONTROLLERAXISMOTION:
                keypad.controllerAxisEvent(event.caxis);
                break;

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                keypad.controllerDeviceEvent(event.cdevice);
                break;

            case SDL_DROPFILE:
                dropEvent(event.drop);
                break;
            }
        }
        frame();

        limiter.frameSleep();

        fps_frame++;
        u32 delta = SDL_GetTicks() - fps_begin;
        if (delta >= 1000)
        {

            std::string fps = fmt::format(" - {:.1f} fps", (1000.f / static_cast<double>(delta) * static_cast<double>(fps_frame)));
            SDL_SetWindowTitle(ppu.backend.window, (title + fps).c_str());

            fps_begin = SDL_GetTicks();
            fps_frame = 0;
        }
    }
}

void Emulator::drawIcon()
{
    SDL_Rect rect = { 0, 0, 240, 160 };
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

bool Emulator::dropAwait()
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
                keypad.controllerDeviceEvent(event.cdevice);
                break;

            case SDL_DROPFILE:
                if (dropEvent(event.drop))
                    return true;
                break;
            }
        }
    }
}

void Emulator::keyboardEvent(const SDL_KeyboardEvent& event)
{
    auto key = event.keysym.sym;

    if (key == config.fullscreen)
        ppu.backend.fullscreen();

    if (key == config.reset)
        reset();

    auto pair = config.fps_map.find(key);
    if (pair != config.fps_map.end())
    {
        limiter.setFPS(pair->second);
    }
}

bool Emulator::dropEvent(const SDL_DropEvent& event)
{
    std::string file(event.file);
    SDL_free(event.file);

    if (!fs::is_regular_file(file))
        return false;

    if (!mmu.gamepak.load(file))
        return false;

    reset();

    updateWindowTitle();
    SDL_RaiseWindow(ppu.backend.window);

    return true;
}

void Emulator::updateWindowTitle()
{
    std::stringstream sstream;
    sstream << "eggvance";
    if (!mmu.gamepak.header.title.empty())
    {
        sstream << " - ";
        sstream << mmu.gamepak.header.title;
    }
    switch (mmu.gamepak.backup->type)
    {
    case Backup::Type::SRAM:     sstream << " - SRAM";     break;
    case Backup::Type::FLASH64:  sstream << " - FLASH64";  break;
    case Backup::Type::FLASH128: sstream << " - FLASH128"; break;
    case Backup::Type::EEPROM:   sstream << " - EEPROM";   break;
    }
    title = sstream.str();
    SDL_SetWindowTitle(ppu.backend.window, title.c_str());
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
