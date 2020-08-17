#include "base/eggcpt.h"

#if EGGCPT_CC_EMSCRIPTEN

#include <emscripten.h>
#include <emscripten/bind.h>

#include "base/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "core/core.h"
#include "core/common.h"
#include "core/framecounter.h"

using namespace emscripten;

FrameCounter counter;

u32 background = 0xFFFFFF;

void idle();
void idleMain()
{
    emscripten_cancel_main_loop();
    emscripten_set_main_loop(idle, 0, 1);
}

void emulate();
void emulateMain(uint fps)
{
    emscripten_cancel_main_loop();
    emscripten_set_main_loop(emulate, fps, 1);
}

void init(int argc, char* argv[])
{
    common::init(argc, argv);
}

template<typename T>
void processInputEvent(const Shortcuts<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        g_core.context.video.fullscreen();

    if (input == shortcuts.fr_hardware)
        emulateMain(kRefreshRate);

    if (input == shortcuts.fr_custom_1)
        emulateMain(config.framerate[0]);

    if (input == shortcuts.fr_custom_2)
        emulateMain(config.framerate[1]);

    if (input == shortcuts.fr_custom_3)
        emulateMain(config.framerate[2]);

    if (input == shortcuts.fr_custom_4)
        emulateMain(config.framerate[3]);

    if (input == shortcuts.fr_unbound)
        emulateMain(6000);
}

void processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
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
        }
    }
}

void idle()
{
    processEvents();
    g_core.context.video.clear(background);
    g_core.context.video.renderIcon();
    SDL_RenderPresent(g_core.context.video.renderer);
}

void emulate()
{
    processEvents();
    keypad.update();
    common::frame();

    double fps = 0;
    if ((++counter).queryFps(fps))
        common::updateWindowTitle(fps);
}

void eggvanceLoadRom(const std::string& filename)
{
    mmu.gamepak.load(filename);
    common::reset();
    common::updateWindowTitle();
    counter = FrameCounter();
    emulateMain(kRefreshRate);
}

void eggvanceLoadSave(const std::string& filename)
{
    mmu.gamepak.loadSave(filename);
    common::reset();
    emulateMain(kRefreshRate);
}

void eggvanceSetBackground(u32 color)
{
    background = color;
}

EMSCRIPTEN_BINDINGS(eggvance)
{
    function("eggvanceLoadRom", &eggvanceLoadRom);
    function("eggvanceLoadSave", &eggvanceLoadSave);
    function("eggvanceSetBackground", &eggvanceSetBackground);
}

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);
        idleMain();
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::puts(ex.what());
        return 1;
    }
}

#endif
