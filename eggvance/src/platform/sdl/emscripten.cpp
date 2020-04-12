#ifdef PLATFORM_EMSCRIPTEN

#include <emscripten.h>
#include <emscripten/bind.h>
#include <fmt/format.h>

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "common/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"
#include "platform/framecounter.h"

using namespace emscripten;

FrameCounter counter;

auto sdl_audio_device = std::make_shared<SDLAudioDevice>();
auto sdl_input_device = std::make_shared<SDLInputDevice>();
auto sdl_video_device = std::make_shared<SDLVideoDevice>();

struct Shortcuts
{
    ShortcutConfig<SDL_Scancode> keyboard;
    ShortcutConfig<SDL_GameControllerButton> controller;
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
void processInput(const ShortcutConfig<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        sdl_video_device->fullscreen();

    if (input == shortcuts.fps_default)
        emulateMain(REFRESH_RATE);

    if (input == shortcuts.fps_custom_1)
        emulateMain(REFRESH_RATE * config.fps_multipliers[0]);

    if (input == shortcuts.fps_custom_2)
        emulateMain(REFRESH_RATE * config.fps_multipliers[1]);

    if (input == shortcuts.fps_custom_3)
        emulateMain(REFRESH_RATE * config.fps_multipliers[2]);

    if (input == shortcuts.fps_custom_4)
        emulateMain(REFRESH_RATE * config.fps_multipliers[3]);

    if (input == shortcuts.fps_unlimited)
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
            processInput(shortcuts.keyboard, event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            processInput(shortcuts.controller, SDL_GameControllerButton(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            sdl_input_device->deviceEvent(event.cdevice);
            break;
        }
    }
}

void idle()
{
    processEvents();
    sdl_video_device->clear(255);
    sdl_video_device->renderIcon();
    SDL_RenderPresent(sdl_video_device->renderer);
}

void emulate()
{
    processEvents();
    keypad.process();
    common::frame();

    double value = 0;
    if ((++counter).queryFps(value))
        sdl_video_device->title(common::title("", value));
}

void eggvanceLoadRom(const std::string& filename)
{
    mmu.gamepak.load(filename);
    common::reset();
    emulateMain(REFRESH_RATE);
}

void eggvanceLoadBackup(const std::string& filename)
{
    mmu.gamepak.loadBackup(filename);
    common::reset();
    emulateMain(REFRESH_RATE);
}

EMSCRIPTEN_BINDINGS(eggvance)
{
    function("eggvanceLoadRom", &eggvanceLoadRom);
    function("eggvanceLoadBackup", &eggvanceLoadBackup);
}

int main(int argc, char* argv[])
{
    init(argc, argv);
    idleMain();
    return 0;
}

#endif
