#include <shell/env.h>

#if SHELL_CC_EMSCRIPTEN

#include <cmath>

#include <emscripten.h>
#include <emscripten/bind.h>

#include "core.h"
#include "framecounter.h"
#include "inputcontext.h"
#include "videocontext.h"
#include "base/config.h"
#include "gamepak/gamepak.h"

using namespace emscripten;

FrameCounter counter;

using Frame = void(*)(void);

void reset()
{
    core::reset();
    core::updateTitle();
    counter = FrameCounter();
}

void setMainLoop(Frame frame, double fps)
{
    emscripten_cancel_main_loop();
    emscripten_set_main_loop(frame, std::lrint(fps), 1);
}

void emulate();

template<typename Input>
void processInputEvent(const Shortcuts<Input>& shortcuts, Input input)
{
    if      (input == shortcuts.reset)       core::reset();
    else if (input == shortcuts.fullscreen)  video_ctx.fullscreen();
    else if (input == shortcuts.fr_hardware) setMainLoop(emulate, kRefreshRate);
    else if (input == shortcuts.fr_custom_1) setMainLoop(emulate, config.framerate[0]);
    else if (input == shortcuts.fr_custom_2) setMainLoop(emulate, config.framerate[1]);
    else if (input == shortcuts.fr_custom_3) setMainLoop(emulate, config.framerate[2]);
    else if (input == shortcuts.fr_custom_4) setMainLoop(emulate, config.framerate[3]);
    else if (input == shortcuts.fr_unbound)  setMainLoop(emulate, 6000);
}

void processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
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
        }
    }
}

void idle()
{
    processEvents();

    video_ctx.renderClear(0xFFFF'FFFF);
    video_ctx.renderIcon();
    video_ctx.renderPresent();
}

void emulate()
{
    processEvents();
    core::frame();

    if (const auto fps = (++counter).fps())
        core::updateTitle(*fps);
}

void eggvanceLoadRom(const std::string& file)
{
    gamepak.loadRom(file, true);
    reset();
    setMainLoop(emulate, kRefreshRate);
}

void eggvanceLoadSave(const std::string& file)
{
    gamepak.loadSave(file);
    reset();
    setMainLoop(emulate, kRefreshRate);
}

EMSCRIPTEN_BINDINGS(eggvance)
{
    function("eggvanceLoadRom", &eggvanceLoadRom);
    function("eggvanceLoadSave", &eggvanceLoadSave);
}

int main(int argc, char* argv[])
{
    try
    {
        core::init(argc, argv);

        setMainLoop(idle, 0);
    }
    catch (const std::exception& ex)
    {
        std::puts(ex.what());
    }
    return 0;
}

#endif
