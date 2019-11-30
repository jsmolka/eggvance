#define SDL_MAIN_HANDLED

#include <filesystem>
#include <memory>
#include <string>
#include <sstream>
#include <fmt/format.h>
#include <SDL2/SDL.h>

#include "arm/arm.h"
#include "common/config.h"
#include "devices/devices.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "system/keypad.h"
#include "framelimiter.h"
#include "icon.h"
#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"

namespace fs = std::filesystem;

std::shared_ptr<SDLAudioDevice> sdl_audio_device;
std::shared_ptr<SDLInputDevice> sdl_input_device;
std::shared_ptr<SDLVideoDevice> sdl_video_device;

// Todo: Move to window
std::string title;

FrameLimiter limiter(59.737);

void reset()
{
    arm.reset();
    ppu.reset();
    mmu.reset();
    keypad.reset();
}

void processShortcut(Config::Shortcut shortcut)
{
    constexpr double hz = 59.737;

    switch (shortcut)
    {
    case Config::Shortcut::Reset:
        reset();
        break;

    case Config::Shortcut::Fullscreen:
        sdl_video_device->fullscreen();
        break;

    case Config::Shortcut::SpeedDefault:
        limiter.setFPS(hz);
        break;

    case Config::Shortcut::SpeedUnlimited:
        limiter.setFPS(hz * 100);
        break;

    case Config::Shortcut::SpeedOption1: limiter.setFPS(hz * config.fps_multipliers[0]); break;
    case Config::Shortcut::SpeedOption2: limiter.setFPS(hz * config.fps_multipliers[1]); break;
    case Config::Shortcut::SpeedOption3: limiter.setFPS(hz * config.fps_multipliers[2]); break;
    case Config::Shortcut::SpeedOption4: limiter.setFPS(hz * config.fps_multipliers[3]); break;
    }
}

void keyboardEvent(const SDL_KeyboardEvent& event)
{
    auto pair = config.shortcuts.keyboard.find(event.keysym.sym);
    if (pair == config.shortcuts.keyboard.end())
        return;

    processShortcut(pair->second);
}

void controllerButtonEvent(const SDL_ControllerButtonEvent& event)
{
    auto pair = config.shortcuts.controller.find(static_cast<SDL_GameControllerButton>(event.button));
    if (pair == config.shortcuts.controller.end())
        return;

    processShortcut(pair->second);
}

void updateWindowTitle()
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
    case Backup::Type::Flash64:  sstream << " - FLASH64";  break;
    case Backup::Type::Flash128: sstream << " - FLASH128"; break;
    case Backup::Type::EEPROM:   sstream << " - EEPROM";   break;
    }
    title = sstream.str();
    SDL_SetWindowTitle(sdl_video_device->window, title.c_str());
}

void frame()
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

void drawIcon()
{
    SDL_Rect rect = { 0, 0, 240, 160 };
    SDL_Renderer* renderer = sdl_video_device->renderer;
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

bool dropEvent(const SDL_DropEvent& event)
{
    std::string file(event.file);
    SDL_free(event.file);

    if (!fs::is_regular_file(file))
        return false;

    if (!mmu.gamepak.load(file))
        return false;

    reset();

    updateWindowTitle();
    SDL_RaiseWindow(sdl_video_device->window);

    return true;
}

bool dropAwait()
{
    drawIcon();

    while (true)
    {
        SDL_Delay(16);
        SDL_RenderPresent(sdl_video_device->renderer);

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

void run()
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

        keypad.update();

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
                //keypad.keyboardEvent(event.key);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                controllerButtonEvent(event.cbutton);
            case SDL_CONTROLLERBUTTONUP:
                //keypad.controllerButtonEvent(event.cbutton);
                break;

            case SDL_CONTROLLERAXISMOTION:
                //keypad.controllerAxisEvent(event.caxis);
                break;

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                //keypad.controllerDeviceEvent(event.cdevice);
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
            SDL_SetWindowTitle(sdl_video_device->window, (title + fps).c_str());

            fps_begin = SDL_GetTicks();
            fps_frame = 0;
        }
    }
}

int main(int argc, char* argv[])
{
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    sdl_audio_device = std::make_shared<SDLAudioDevice>();
    sdl_input_device = std::make_shared<SDLInputDevice>();
    sdl_video_device = std::make_shared<SDLVideoDevice>();

    audio_device = sdl_audio_device;
    input_device = sdl_input_device;
    video_device = sdl_video_device;

    try
    {
        config.init();
        mmu.bios.init();
        keypad.init();

        sdl_audio_device->init();
        sdl_input_device->init();
        sdl_video_device->init();

        if (argc > 1)
            mmu.gamepak.load(argv[1]);

        run();
    }
    catch (std::exception ex)
    {
        return 1;
    }
    return 0;
}
