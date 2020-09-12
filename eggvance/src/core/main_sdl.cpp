#include <eggcpt/env.h>

#if !EGGCPT_CC_EMSCRIPTEN

#include <stdexcept>

#include "base/constants.h"
#include "base/config.h"
#include "core/core.h"
#include "core/framecounter.h"
#include "core/synchronizer.h"
#include "core/inputcontext.h"
#include "core/videocontext.h"
#include "mmu/mmu.h"

bool running = true;
FrameCounter counter;
Synchronizer synchronizer(kRefreshRate);

void processDropEvent(const SDL_DropEvent& event)
{
    auto file = fs::u8path(event.file);

    SDL_free(event.file);

    video_ctx.raise();

    if (file.extension() == ".sav")
    {
        mmu.gamepak.loadSave(file);
        core::reset();
    }
    else
    {
        mmu.gamepak.load(file);
        core::reset();
        core::updateTitle();
        counter = FrameCounter();
    }
}

template<typename T>
void processInputEvent(const Shortcuts<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        core::reset();

    if (input == shortcuts.fullscreen)
        video_ctx.fullscreen();

    if (input == shortcuts.fr_hardware)
        synchronizer = Synchronizer(kRefreshRate);

    if (input == shortcuts.fr_custom_1)
        synchronizer = Synchronizer(config.framerate[0]);

    if (input == shortcuts.fr_custom_2)
        synchronizer = Synchronizer(config.framerate[1]);

    if (input == shortcuts.fr_custom_3)
        synchronizer = Synchronizer(config.framerate[2]);

    if (input == shortcuts.fr_custom_4)
        synchronizer = Synchronizer(config.framerate[3]);

    if (input == shortcuts.fr_unbound)
        synchronizer = Synchronizer(6000);
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
            processInputEvent(config.shortcuts.keyboard, event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            processInputEvent(config.shortcuts.controller, SDL_GameControllerButton(event.cbutton.button));
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

void emulate()
{
    while (running && mmu.gamepak.size() == 0)
    {
        processEvents();

        video_ctx.renderClear(0x2B3137);
        video_ctx.renderIcon();
        video_ctx.renderPresent();

        SDL_Delay(16);
    }

    core::reset();
    core::updateTitle();

    counter = FrameCounter();

    while (running)
    {
        synchronizer.sync([]()
        {
            processEvents();
            core::frame();
        });

        if (auto fps = (++counter).fps())
            core::updateTitle(*fps);
    }
}

int main(int argc, char* argv[])
{
    fs::setBasePath(SDL_GetBasePath());
    try
    {
        config = Config(fs::makeAbsolute("eggvance.toml"));

        core::init(argc, argv);
        emulate();
        return 0;
    }
    catch (const std::exception& ex)
    {
        SDL_ShowSimpleMessageBox(0, "Exception", ex.what(), NULL);
        return 1;
    }
}

#endif