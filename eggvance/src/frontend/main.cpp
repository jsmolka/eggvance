#include <shell/fmt.h>
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
FrameCounter counter;
FrameRateLimiter limiter(kRefreshRate);

void updateTitle()
{
    const auto title = fmt::format(
        gamepak.rom.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        gamepak.rom.title);

    video_ctx.title(title);
}

void updateTitle(double fps)
{
    const auto title = fmt::format(
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

void processDropEvent(const SDL_DropEvent& event)
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
void processInputEvent(const Shortcuts<Input>& shortcuts, Input input)
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

void processEvents()
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
            processInputEvent(
                config.shortcuts.keyboard,
                event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            processInputEvent(
                config.shortcuts.controller,
                static_cast<SDL_GameControllerButton>(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.processDeviceEvent(event.cdevice);
            break;

        case SDL_DROPFILE:
            processDropEvent(event.drop);
            break;
        }
    }
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

        gamepak.load(
            gba.value_or(fs::path()),
            sav.value_or(fs::path()));
    }
    catch (const ParseError& error)
    {
        fmt::print(options.help());
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
            processEvents();

            video_ctx.renderClear(0xFF3E'4750);
            video_ctx.renderIcon();
            video_ctx.renderPresent();

            SDL_Delay(16);
        }

        if (!running)
            return 0;

        reset();

        counter = FrameCounter();

        audio_ctx.unpause();

        while (running)
        {
            limiter.run([]() 
            {
                constexpr auto kPixelsHor   = 240 + 68;
                constexpr auto kPixelsVer   = 160 + 68;
                constexpr auto kPixelCycles = 4;
                constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

                processEvents();
                keypad.update();

                arm.run(kFrameCycles);

                ppu.present();
            });

            if (const auto fps = (++counter).fps())
                updateTitle(*fps);
        }

        audio_ctx.pause();
        audio_ctx.clear();
    }
    catch (const std::exception& ex)
    {
        panic(ex.what());
    }
    return 0;
}
