#ifdef PLATFORM_SDL

#include <stdexcept>

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "base/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"
#include "platform/framecounter.h"
#include "platform/synchronizer.h"

bool running = true;
FrameCounter counter;
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

void processDropEvent(const SDL_DropEvent& event)
{
    auto file = fs::u8path(event.file);

    SDL_free(event.file);
    SDL_RaiseWindow(sdl_video_device->window);

    if (file.extension() == ".gba")
    {
        mmu.gamepak.load(file);
        common::reset();
        common::updateWindowTitle();
        counter = FrameCounter();
    }
    else
    {
        mmu.gamepak.loadBackup(file);
        common::reset();
    }
}

template<typename T>
void processInputEvent(const ShortcutConfig<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        common::reset();

    if (input == shortcuts.fullscreen)
        sdl_video_device->fullscreen();

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
            processInputEvent(shortcuts.keyboard, event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            processInputEvent(shortcuts.controller, SDL_GameControllerButton(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            sdl_input_device->processDeviceEvent(event.cdevice);
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
        sdl_video_device->clear(0x2B3137);
        sdl_video_device->renderIcon();
        SDL_RenderPresent(sdl_video_device->renderer);
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
            keypad.process();
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
    catch (const std::runtime_error& error)
    {
        SDL_ShowSimpleMessageBox(0, "Error", error.what(), nullptr);
        return 1;
    }
}

#endif
