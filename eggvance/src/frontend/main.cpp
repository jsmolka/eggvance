#include <shell/format.h>
#include <shell/options.h>
#include <shell/utility.h>

#include "audiocontext.h"
#include "framecounter.h"
#include "frameratelimiter.h"
#include "inputcontext.h"
#include "videocontext.h"
#include "apu/apu.h"
#include "arm/arm.h"
#include "base/config.h"
#include "base/constants.h"
#include "base/panic.h"
#include "dma/dma.h"
#include "gamepak/gamepak.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"
#include "scheduler/scheduler.h"
#include "sio/sio.h"
#include "timer/timer.h"

bool running = true;
bool changed = false;
FrameCounter counter;
FrameRateLimiter limiter(kRefreshRate);

void updateTitle()
{
    const auto title = shell::format(
        gamepak.rom.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        gamepak.rom.title);

    video_ctx.title(title);
}

void updateTitle(double fps)
{
    const auto title = shell::format(
        gamepak.rom.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        gamepak.rom.title, fps);

    video_ctx.title(title);
}

void reset()
{
    gamepak.gpio->reset();
    gamepak.save->reset();

    shell::reconstruct(apu);
    shell::reconstruct(arm);
    shell::reconstruct(dma);
    shell::reconstruct(ppu);
    shell::reconstruct(keypad);
    shell::reconstruct(sio);
    shell::reconstruct(scheduler);
    shell::reconstruct(timer);

    arm.init();
    apu.init();
    ppu.init();

    updateTitle();
}

void handleDropEvent(const SDL_DropEvent& event)
{
    const fs::path file = fs::u8path(event.file);

    SDL_free(event.file);

    if (file.extension() == ".gba")
    {
        audio_ctx.pause();
        audio_ctx.clear();

        gamepak.load(file, fs::path());
        reset();

        audio_ctx.unpause();
    }
    
    if (file.extension() == ".sav")
    {
        audio_ctx.pause();
        audio_ctx.clear();

        gamepak.load(fs::path(), file);
        reset();

        audio_ctx.unpause();
    }

    video_ctx.raise();
}

template<typename Input>
void handleInputEvent(const Shortcuts<Input>& shortcuts, Input input)
{
    if      (input == shortcuts.reset)       reset();
    else if (input == shortcuts.fullscreen)  video_ctx.fullscreen();
    else if (input == shortcuts.fr_hardware) limiter = FrameRateLimiter(kRefreshRate);
    else if (input == shortcuts.fr_custom_1) limiter = FrameRateLimiter(config.framerate[0]);
    else if (input == shortcuts.fr_custom_2) limiter = FrameRateLimiter(config.framerate[1]);
    else if (input == shortcuts.fr_custom_3) limiter = FrameRateLimiter(config.framerate[2]);
    else if (input == shortcuts.fr_custom_4) limiter = FrameRateLimiter(config.framerate[3]);
    else if (input == shortcuts.fr_unbound)  limiter = FrameRateLimiter(6000);
}

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            handleInputEvent(
                config.shortcuts.keyboard,
                event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            handleInputEvent(
                config.shortcuts.controller,
                static_cast<SDL_GameControllerButton>(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.processDeviceEvent(event.cdevice);
            break;

        case SDL_DROPFILE:
            handleDropEvent(event.drop);
            break;
        }
    }
}

int eventFilter(void*, SDL_Event* event)
{
    if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED)
    {
        if (gamepak.rom.size == 0)
        {
            video_ctx.renderClear(0xFF3E'4750);
            video_ctx.renderIcon();
            video_ctx.renderPresent();
        }
        else
        {
            video_ctx.renderCopyTexture();
            video_ctx.renderPresent();
        }
        return 0;
    }
    #if SHELL_OS_WINDOWS
    else if (event->type == SDL_SYSWMEVENT && event->syswm.msg->msg.win.msg == WM_EXITSIZEMOVE)
    {
        changed = true;
        return 0;
    }
    #endif

    return 1;
}

void init(int argc, char* argv[])
{
    using namespace shell;

    Options options("eggvance");
    options.add({ "-c,--config", "config file", "file" }, Options::value<fs::path>("eggvance.ini"));
    options.add({ "-s,--save",   "save file",   "file" }, Options::value<fs::path>()->optional());
    options.add({       "rom",   "ROM file"            }, Options::value<fs::path>()->positional()->optional());

    try
    {
        OptionsResult result = options.parse(argc, argv);

        const auto cfg = result.find<fs::path>("--config");
        const auto sav = result.find<fs::path>("--save");
        const auto gba = result.find<fs::path>("rom");

        config.init(fs::absolute(*cfg));

        Bios::init(config.bios_file);

        audio_ctx.init();
        input_ctx.init();
        video_ctx.init();

        #if SHELL_OS_WINDOWS
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
        #endif
        SDL_SetEventFilter(eventFilter, NULL);

        gamepak.load(
            gba.value_or(fs::path()),
            sav.value_or(fs::path()));
    }
    catch (const ParseError& error)
    {
        shell::print(options.help());
        panic("Cannot parse command line\n{}", error.what());
    }
}

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);

        while (running && gamepak.rom.size == 0)
        {
            handleEvents();

            video_ctx.renderClear(0xFF3E'4750);
            video_ctx.renderIcon();
            video_ctx.renderPresent();

            SDL_Delay(16);
        }

        if (!running)
            return 0;

        reset();

        counter.reset();

        audio_ctx.unpause();

        while (running)
        {
            limiter.run([]() 
            {
                constexpr auto kPixelsHor   = 240 + 68;
                constexpr auto kPixelsVer   = 160 + 68;
                constexpr auto kPixelCycles = 4;
                constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

                handleEvents();
                keypad.update();

                arm.run(kFrameCycles);
                ppu.present();
            });

            if (changed)
            {
                counter.reset();
                limiter.reset();
                changed = false;
            }
            else
            {
                if (const auto fps = (++counter).fps())
                    updateTitle(*fps);
            }
        }

        audio_ctx.pause();
    }
    catch (const std::exception& ex)
    {
        panic(ex.what());
    }
    return 0;
}
