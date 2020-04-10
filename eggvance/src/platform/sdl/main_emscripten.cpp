#ifdef PLATFORM_EMSCRIPTEN

#include <emscripten.h>

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "common/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"

auto sdl_audio_device = std::make_shared<SDLAudioDevice>();
auto sdl_input_device = std::make_shared<SDLInputDevice>();
auto sdl_video_device = std::make_shared<SDLVideoDevice>();

struct Shortcuts
{
    ShortcutConfig<SDL_Scancode> keyboard;
    ShortcutConfig<SDL_GameControllerButton> controller;
} shortcuts;

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
void processInput(const ShortcutConfig<T>& config, T input)
{
    if (input == config.reset)
        common::reset();

    if (input == config.fullscreen)
        sdl_video_device->fullscreen();
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

void icon()
{
    int w;
    int h;
    SDL_RenderGetLogicalSize(sdl_video_device->renderer, &w, &h);
    SDL_RenderSetLogicalSize(sdl_video_device->renderer, 18, 18);

    sdl_video_device->renderIcon();
    SDL_RenderPresent(sdl_video_device->renderer);

    SDL_RenderSetLogicalSize(sdl_video_device->renderer, w, h);
}

void emulate()
{
    processEvents();
    keypad.process();
    common::frame();
}

#ifdef __cplusplus
extern "C" {
    #endif

    EMSCRIPTEN_KEEPALIVE void eggvanceLoadRom(const char* filename)
    {
        mmu.gamepak.load(filename);
        common::reset();

        emscripten_cancel_main_loop();
        emscripten_set_main_loop(emulate, 0, 1);
    }

    EMSCRIPTEN_KEEPALIVE void eggvanceUnloadRom(const char* filename)
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
    emscripten_set_main_loop(icon, 0, 1);
    return 0;
}

#endif
