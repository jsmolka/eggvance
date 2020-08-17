#include "base/eggcpt.h"

#if !EGGCPT_CC_EMSCRIPTEN

#include <stdexcept>

#include "base/config.h"
#include "core/core.h"
#include "core/common.h"
#include "core/framecounter.h"
#include "core/synchronizer.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"

bool running = true;
FrameCounter counter;
Synchronizer synchronizer;

void init(int argc, char* argv[])
{
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    common::init(argc, argv);
}

void processDropEvent(const SDL_DropEvent& event)
{
    auto file = fs::u8path(event.file);

    SDL_free(event.file);
    SDL_RaiseWindow(g_core.context.video.window);

    if (file.extension() == ".gba")
    {
        mmu.gamepak.load(file);
        common::reset();
        common::updateWindowTitle();
        counter = FrameCounter();
    }
    else
    {
        mmu.gamepak.loadSave(file);
        common::reset();
    }
}

template<typename T>
void processInputEvent(const Shortcuts<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        g_core.context.video.fullscreen();

    if (input == shortcuts.fr_hardware)
        synchronizer.setFps(kRefreshRate);

    if (input == shortcuts.fr_custom_1)
        synchronizer.setFps(config.framerate[0]);

    if (input == shortcuts.fr_custom_2)
        synchronizer.setFps(config.framerate[1]);

    if (input == shortcuts.fr_custom_3)
        synchronizer.setFps(config.framerate[2]);

    if (input == shortcuts.fr_custom_4)
        synchronizer.setFps(config.framerate[3]);

    if (input == shortcuts.fr_unbound)
        synchronizer.setFps(6000);
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
            g_core.context.input.processDeviceEvent(event.cdevice);
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
        g_core.context.video.clear(0x2B3137);
        g_core.context.video.renderIcon();
        SDL_RenderPresent(g_core.context.video.renderer);
        SDL_Delay(16);
    }

    common::reset();
    common::updateWindowTitle();

    counter = FrameCounter();

    while (running)
    {
        synchronizer.synchronize([]()
        {
            processEvents();
            keypad.update();
            common::frame();
        });

        double fps = 0;
        if ((++counter).queryFps(fps))
            common::updateWindowTitle(fps);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);
        emulate();
        return 0;
    }
    catch (const std::exception& ex)
    {
        SDL_ShowSimpleMessageBox(0, "Exception", ex.what(), nullptr);
        return 1;
    }
}

#endif