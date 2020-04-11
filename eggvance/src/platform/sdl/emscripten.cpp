#define PLATFORM_EMSCRIPTEN
#ifdef PLATFORM_EMSCRIPTEN

#include <emscripten.h>
#include <fmt/format.h>

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "common/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"
#include "platform/framecounter.h"

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
void idleLoop(uint fps)
{
    emscripten_cancel_main_loop();
    emscripten_set_main_loop(idle, fps, 1);
}

void emulate();
void emulateLoop(uint fps)
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
    shortcuts.keyboard.fullscreen = SDL_SCANCODE_T;
}

template<typename T>
void processInput(const ShortcutConfig<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        sdl_video_device->fullscreen();

    if (input == shortcuts.fps_default)
        emulateLoop(REFRESH_RATE);

    if (input == shortcuts.fps_custom_1)
        emulateLoop(REFRESH_RATE * config.fps_multipliers[0]);

    if (input == shortcuts.fps_custom_2)
        emulateLoop(REFRESH_RATE * config.fps_multipliers[1]);

    if (input == shortcuts.fps_custom_3)
        emulateLoop(REFRESH_RATE * config.fps_multipliers[2]);

    if (input == shortcuts.fps_custom_4)
        emulateLoop(REFRESH_RATE * config.fps_multipliers[3]);

    if (input == shortcuts.fps_unlimited)
        emulateLoop(1000);
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
            std::printf("controller");
            sdl_input_device->deviceEvent(event.cdevice);
            break;
        }
    }
}

void idle()
{
    int w;
    int h;
    SDL_RenderGetLogicalSize(sdl_video_device->renderer, &w, &h);
    SDL_RenderSetLogicalSize(sdl_video_device->renderer, 18, 18);

    sdl_video_device->renderIcon();
    SDL_RenderPresent(sdl_video_device->renderer);
    processEvents();

    SDL_RenderSetLogicalSize(sdl_video_device->renderer, w, h);
}

void emulate()
{
    processEvents();
    keypad.process();
    common::frame();

    double value = 0;
    if ((++counter).queryFps(value))
        sdl_video_device->title(fmt::format("eggvance - {:.1f} fps", value));
}

#ifdef __cplusplus
extern "C" {
#endif

    EMSCRIPTEN_KEEPALIVE void loadRom(const char* filename)
    {
        mmu.gamepak.load(filename);
        common::reset();
        emulateLoop(REFRESH_RATE);
    }

    EMSCRIPTEN_KEEPALIVE void loadBackup(const char* filename)
    {
        mmu.gamepak.loadBackup(filename);
        common::reset();
        emulateLoop(REFRESH_RATE);
    }

    EMSCRIPTEN_KEEPALIVE void removeFile(const char* filename)
    {
        if (std_filesystem::exists(filename))
            std_filesystem::remove(filename);
    }

#ifdef __cplusplus
}
#endif

int main(int argc, char* argv[])
{
    init(argc, argv);
    idleLoop(60);
    return 0;
}

#endif
