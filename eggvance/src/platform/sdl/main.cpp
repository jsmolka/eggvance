#include <fmt/format.h>

#include "arm/arm.h"
#include "common/config.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "system/keypad.h"
#include "framelimiter.h"
#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"

std::shared_ptr<SDLAudioDevice> sdl_audio_device;
std::shared_ptr<SDLInputDevice> sdl_input_device;
std::shared_ptr<SDLVideoDevice> sdl_video_device;

FrameLimiter limiter(REFRESH_RATE);

std::string window_title;

struct Shortcuts
{
    ShortcutConfig<SDL_Scancode> keyboard;
    ShortcutConfig<SDL_GameControllerButton> controller;
} shortcuts;

void init()
{
    config.init();

    mmu.bios.init(config.bios_file);

    sdl_audio_device->init();
    sdl_input_device->init();
    sdl_video_device->init();

    shortcuts.keyboard = config.shortcuts.keyboard.map<SDL_Scancode>(SDLInputDevice::mapKey);
    shortcuts.controller = config.shortcuts.controller.map<SDL_GameControllerButton>(SDLInputDevice::mapButton);
}

void reset()
{
    arm.reset();
    ppu.reset();
    mmu.reset();
    keypad.reset();
}

template<typename T>
void processInput(const ShortcutConfig<T>& config, T input)
{
    if (input == config.reset)
        reset();

    if (input == config.fullscreen)
        video_device->fullscreen();

    if (input == config.fps_default)
        limiter.setFPS(REFRESH_RATE);

    if (input == config.fps_custom_1)
        limiter.setFPS(REFRESH_RATE * ::config.fps_multipliers[0]);

    if (input == config.fps_custom_2)
        limiter.setFPS(REFRESH_RATE * ::config.fps_multipliers[1]);

    if (input == config.fps_custom_3)
        limiter.setFPS(REFRESH_RATE * ::config.fps_multipliers[2]);

    if (input == config.fps_custom_4)
        limiter.setFPS(REFRESH_RATE * ::config.fps_multipliers[3]);

    if (input == config.fps_unlimited)
        limiter.setFPS(REFRESH_RATE * 1000);
}

void updateWindowTitle()
{
    window_title = "eggvance";

    if (!mmu.gamepak.header.title.empty())
    {
        window_title += fmt::format(" - {}", mmu.gamepak.header.title);
    }

    sdl_video_device->setWindowTitle(window_title);
}

bool dropEvent(const SDL_DropEvent& event)
{
    std::string file(event.file);
    SDL_free(event.file);

    if (!mmu.gamepak.load(file))
        return false;

    reset();

    sdl_video_device->raiseWindow();

    return true;
}

bool dropMain()
{
    sdl_video_device->drawIcon();

    while (true)
    {
        SDL_Delay(16);
        SDL_RenderPresent(sdl_video_device->renderer);

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
}

void frame()
{
    for (int line = 0; line < SCREEN_H; ++line)
    {
        arm.run(960);
        ppu.scanline();
        ppu.hblank();
        arm.run(272);
        ppu.next();
    }

    ppu.vblank();
    for (int line = 0; line < 68; ++line)
    {
        arm.run(1232);
        ppu.next();
    }

    ppu.present();
}

void emulate()
{
    if (mmu.gamepak.size() == 0)
    {
        if (!dropMain())
            return;
    }

    updateWindowTitle();

    u32 fps_frame = 0;
    u32 fps_begin = SDL_GetTicks();

    while (true)
    {
        limiter.frameBegin();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return;

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

        keypad.update();

        frame();

        limiter.frameSleep();

        fps_frame++;
        u32 delta = SDL_GetTicks() - fps_begin;
        if (delta >= 1000)
        {
            sdl_video_device->setWindowTitle(
                window_title + fmt::format(" - {:.1f} fps", (1000.f / double(delta) * double(fps_frame)))
            );
            fps_begin = SDL_GetTicks();
            fps_frame = 0;
        }
    }
}

int main(int argc, char* argv[])
{
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    sdl_audio_device = std::make_shared<SDLAudioDevice>();
    sdl_input_device = std::make_shared<SDLInputDevice>();
    sdl_video_device = std::make_shared<SDLVideoDevice>();

    audio_device = sdl_audio_device;
    input_device = sdl_input_device;
    video_device = sdl_video_device;

    try
    {
        init();

        if (argc > 1)
            mmu.gamepak.load(argv[1]);

        reset();

        emulate();
    }
    catch (std::exception ex)
    {
        SDL_ShowSimpleMessageBox(0, "Error", ex.what(), nullptr);
    }
    return 0;
}
