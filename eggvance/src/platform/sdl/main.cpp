#include <stdexcept>
#include <fmt/format.h>

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "common/config.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "platform/common.h"
#include "platform/framecounter.h"
#include "platform/synchronizer.h"

#ifdef _MSC_VER
#define MAIN SDL_main
#else
#define MAIN main
#endif

bool running = true;
Synchronizer synchronizer;
std::string title = "eggvance";

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

bool dropEvent(const SDL_DropEvent& event)
{
    Path file(event.file);

    SDL_free(event.file);
    SDL_RaiseWindow(sdl_video_device->window);

    return mmu.gamepak.load(file);
}

bool dropAwait()
{
    int w;
    int h;
    SDL_RenderGetLogicalSize(sdl_video_device->renderer, &w, &h);
    SDL_RenderSetLogicalSize(sdl_video_device->renderer, 18, 18);

    bool running = [&]()
    {
        while (true)
        {
            sdl_video_device->renderIcon();
            SDL_RenderPresent(sdl_video_device->renderer);
            SDL_Delay(16);

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    return false;

                case SDL_CONTROLLERDEVICEADDED:
                case SDL_CONTROLLERDEVICEREMOVED:
                    sdl_input_device->deviceEvent(event.cdevice);
                    break;

                case SDL_DROPFILE:
                    if (dropEvent(event.drop))
                        return true;
                    break;
                }
            }
        }
    }();

    SDL_RenderSetLogicalSize(sdl_video_device->renderer, w, h);

    return running;
}

template<typename T>
void processInput(const ShortcutConfig<T>& config, T input)
{
    if (input == config.reset)
        common::reset();

    if (input == config.fullscreen)
        sdl_video_device->fullscreen();

    if (input == config.fps_default)
        synchronizer.setFps(REFRESH_RATE);

    if (input == config.fps_custom_1)
        synchronizer.setFps(REFRESH_RATE * ::config.fps_multipliers[0]);

    if (input == config.fps_custom_2)
        synchronizer.setFps(REFRESH_RATE * ::config.fps_multipliers[1]);

    if (input == config.fps_custom_3)
        synchronizer.setFps(REFRESH_RATE * ::config.fps_multipliers[2]);

    if (input == config.fps_custom_4)
        synchronizer.setFps(REFRESH_RATE * ::config.fps_multipliers[3]);

    if (input == config.fps_unlimited)
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
            if (dropEvent(event.drop))
               common::reset();
            break;
        }
    }
}

void updateTitle()
{
    title = mmu.gamepak.header.title.empty()
        ? "eggvance"
        : "eggvance - " + mmu.gamepak.header.title;

    sdl_video_device->title(title);
}

void loop()
{
    if (mmu.gamepak.size() == 0)
        running = dropAwait();

    common::reset();
    updateTitle();

    FrameCounter counter;

    while (running)
    {
        synchronizer.beginFrame();

        processEvents();
        keypad.process();
        common::frame();

        synchronizer.endFrame();

        double value = 0;
        if ((++counter).queryFps(value))
            sdl_video_device->title(fmt::format("{} - {:.1f} fps", title, value));
    }
}

int MAIN(int argc, char* argv[])
{
    try
    {
        init(argc, argv);
        loop();
        return 0;
    }
    catch (const std::runtime_error& error)
    {
        SDL_ShowSimpleMessageBox(0, "Error", error.what(), nullptr);
        return 1;
    }
}
