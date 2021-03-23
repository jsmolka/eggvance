#include "core.h"
#include "audiocontext.h"
#include "framecounter.h"
#include "frameratelimiter.h"
#include "inputcontext.h"
#include "videocontext.h"
#include "base/config.h"
#include "base/panic.h"
#include "gamepak/gamepak.h"

bool running = true;
FrameCounter counter;
FrameRateLimiter limiter(kRefreshRate);

void reset()
{
    core::reset();
    core::updateTitle();
    counter = FrameCounter();
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
    if      (input == shortcuts.reset)       core::reset();
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

int main(int argc, char* argv[])
{
    try
    {
        core::init(argc, argv);

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

        audio_ctx.unpause();

        while (running)
        {
            limiter.run([]() {
                processEvents();
                core::frame();
            });

            if (const auto fps = (++counter).fps())
                core::updateTitle(*fps);
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
