#ifdef PLATFORM_EMSCRIPTEN

#include <emscripten.h>
#include <emscripten/bind.h>

#include "base/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"
#include "platform/framecounter.h"
#include "platform/sdl/sdlaudiodevice.h"
#include "platform/sdl/sdlinputdevice.h"
#include "platform/sdl/sdlvideodevice.h"

using namespace emscripten;

FrameCounter counter;

u32 background = 0xFFFFFF;

auto sdl_audio_device = std::make_shared<SDLAudioDevice>();
auto sdl_input_device = std::make_shared<SDLInputDevice>();
auto sdl_video_device = std::make_shared<SDLVideoDevice>();

struct
{
    Shortcuts<SDL_Scancode> keyboard;
    Shortcuts<SDL_GameControllerButton> controller;
} shortcuts;

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
    common::init(
        argc, argv,
        sdl_audio_device,
        sdl_input_device,
        sdl_video_device
    );

    shortcuts.keyboard = config.shortcuts.keyboard.convert<SDL_Scancode>(SDLInputDevice::convertKey);
    shortcuts.controller = config.shortcuts.controller.convert<SDL_GameControllerButton>(SDLInputDevice::convertButton);
}

template<typename T>
void processInputEvent(const Shortcuts<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        sdl_video_device->fullscreen();

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
            processInputEvent(shortcuts.keyboard, event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            processInputEvent(shortcuts.controller, SDL_GameControllerButton(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            sdl_input_device->processDeviceEvent(event.cdevice);
            break;
        }
    }
}

void idle()
{
    processEvents();
    sdl_video_device->clear(background);
    sdl_video_device->renderIcon();
    SDL_RenderPresent(sdl_video_device->renderer);
}

void emulate()
{
    processEvents();
    keypad.process();
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
    init(argc, argv);
    idleMain();
    return 0;
}

#endif
