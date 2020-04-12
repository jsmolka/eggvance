#ifdef PLATFORM_SDL

#include <stdexcept>

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "common/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"
#include "platform/framecounter.h"
#include "platform/synchronizer.h"

bool running = true;
Synchronizer synchronizer;

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
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    common::init(
        argc, argv,
        sdl_audio_device,
        sdl_input_device,
        sdl_video_device
    );

    shortcuts.keyboard = config.shortcuts.keyboard.convert<SDL_Scancode>(SDLInputDevice::convertKey);
    shortcuts.controller = config.shortcuts.controller.convert<SDL_GameControllerButton>(SDLInputDevice::convertButton);
}

void dropEvent(const SDL_DropEvent& event)
{
    Path file(event.file);

    SDL_free(event.file);
    SDL_RaiseWindow(sdl_video_device->window);

    if (file.extension() == ".gba")
        mmu.gamepak.load(file);
    else
        mmu.gamepak.loadBackup(file);

    common::reset();
}

template<typename T>
void processInput(const ShortcutConfig<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        sdl_video_device->fullscreen();

    if (input == shortcuts.fps_default)
        synchronizer.setFps(REFRESH_RATE);

    if (input == shortcuts.fps_custom_1)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[0]);

    if (input == shortcuts.fps_custom_2)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[1]);

    if (input == shortcuts.fps_custom_3)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[2]);

    if (input == shortcuts.fps_custom_4)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[3]);

    if (input == shortcuts.fps_unlimited)
        synchronizer.setFps(REFRESH_RATE * 1000);
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
            processInput(shortcuts.keyboard, event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            processInput(shortcuts.controller, SDL_GameControllerButton(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            sdl_input_device->deviceEvent(event.cdevice);
            break;

        case SDL_DROPFILE:
            dropEvent(event.drop);
            break;
        }
    }
}

void emulate()
{
    while (running && mmu.gamepak.size() == 0)
    {
        processEvents();
        sdl_video_device->clear(56);
        sdl_video_device->renderIcon();
        SDL_RenderPresent(sdl_video_device->renderer);
        SDL_Delay(16);
    }

    common::reset();

    FrameCounter counter;

    while (running)
    {
        synchronizer.synchronize([](){
            processEvents();
            keypad.process();
            common::frame();
        });

        double value = 0;
        if ((++counter).queryFps(value))
            sdl_video_device->title(common::title(mmu.gamepak.header.title, value));
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
        SDL_ShowSimpleMessageBox(0, "Error", ex.what(), nullptr);
        return 1;
    }
}

#endif
