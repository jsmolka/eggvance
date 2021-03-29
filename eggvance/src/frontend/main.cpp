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
#include "dma/dma.h"
#include "gamepak/gamepak.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"
#include "scheduler/scheduler.h"
#include "sio/sio.h"
#include "timer/timer.h"

bool running = true;
bool paused  = false;
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
    const auto file = fs::u8path(event.file);

    SDL_free(event.file);

    audio_ctx.pause();

    if (file.extension() == ".gba")
    {
        gamepak.load(file, fs::path());
        reset();
    }
    if (file.extension() == ".sav")
    {
        gamepak.load(fs::path(), file);
        reset();
    }

    audio_ctx.unpause();
    video_ctx.raise();
}

template<typename Input>
void handleInputEvent(const Shortcuts<Input>& shortcuts, Input input)
{
    if      (input == shortcuts.reset)          reset();
    else if (input == shortcuts.pause)          paused ^= true;
    else if (input == shortcuts.fullscreen)     video_ctx.fullscreen();
    else if (input == shortcuts.volume_up)      config.volume = std::clamp(config.volume + config.volume_step, 0.0, 1.0);
    else if (input == shortcuts.volume_down)    config.volume = std::clamp(config.volume - config.volume_step, 0.0, 1.0);
    else if (input == shortcuts.speed_hardware) limiter = FrameRateLimiter(kRefreshRate);
    else if (input == shortcuts.speed_2x)       limiter = FrameRateLimiter(kRefreshRate * 2);
    else if (input == shortcuts.speed_4x)       limiter = FrameRateLimiter(kRefreshRate * 4);
    else if (input == shortcuts.speed_6x)       limiter = FrameRateLimiter(kRefreshRate * 6);
    else if (input == shortcuts.speed_8x)       limiter = FrameRateLimiter(kRefreshRate * 8);
    else if (input == shortcuts.speed_unbound)  limiter = FrameRateLimiter(1'000'000);
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
                SDL_GameControllerButton(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.handleDeviceEvent(event.cdevice);
            break;

        case SDL_DROPFILE:
            handleDropEvent(event.drop);
            break;
        }
    }
}

#if SHELL_OS_WINDOWS
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
    else if (event->type == SDL_SYSWMEVENT && event->syswm.msg->msg.win.msg == WM_EXITSIZEMOVE)
    {
        changed = true;
        return 0;
    }
    return 1;
}
#endif

void init(int argc, char* argv[])
{
    using namespace shell;

    Options options("eggvance");
    options.add({ "-c,--config", "config file", "file" }, Options::value<fs::path>("eggvance.ini"));
    options.add({ "-s,--save",   "save file",   "file" }, Options::value<fs::path>()->optional());
    options.add({       "rom",   "ROM file"            }, Options::value<fs::path>()->positional()->optional());

    OptionsResult result;

    try
    {
        result = options.parse(argc, argv);
    }
    catch (const ParseError& error)
    {
        throw shell::Error("Cannot parse command line: {}", error.what());
    }

    const auto cfg = result.find<fs::path>("--config");
    const auto sav = result.find<fs::path>("--save");
    const auto gba = result.find<fs::path>("rom");

    config.init(fs::absolute(*cfg));

    audio_ctx.init();
    input_ctx.init();
    video_ctx.init();

    Bios::init(config.bios_file);

    #if SHELL_OS_WINDOWS
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    SDL_SetEventFilter(eventFilter, NULL);
    #endif

    gamepak.load(
        gba.value_or(fs::path()),
        sav.value_or(fs::path()));
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
                handleEvents();

                if (!paused)
                {
                    constexpr auto kPixelsHor   = 240 + 68;
                    constexpr auto kPixelsVer   = 160 + 68;
                    constexpr auto kPixelCycles = 4;
                    constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

                    keypad.update();
                    arm.run(kFrameCycles);
                    ppu.present();
                }
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
        showMessageBox("Exception", ex.what());
    }
    return 0;
}
