#include "sdlplatform.h"

#include "common/config.h"
#include "mmu/mmu.h"
#include "platform/audiodevice.h"
#include "platform/inputdevice.h"
#include "platform/videodevice.h"

void SDLPlatform::initHookBeg()
{
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    sdl_audio_device = std::make_shared<SDLAudioDevice>();
    sdl_input_device = std::make_shared<SDLInputDevice>();
    sdl_video_device = std::make_shared<SDLVideoDevice>();

    audio_device = sdl_audio_device;
    input_device = sdl_input_device;
    video_device = sdl_video_device;
}

void SDLPlatform::initHookEnd()
{
    shortcuts.keyboard = config.shortcuts.keyboard.convert<SDL_Scancode>(SDLInputDevice::convertKey);
    shortcuts.controller = config.shortcuts.controller.convert<SDL_GameControllerButton>(SDLInputDevice::convertButton);
}

void SDLPlatform::mainHookBeg()
{
    if (mmu.gamepak.size() == 0)
        running = dropAwait();
}

void SDLPlatform::mainHookEnd()
{

}

void SDLPlatform::processEvents()
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
                reset();
            break;
        }
    }
}

bool SDLPlatform::dropAwait()
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

bool SDLPlatform::dropEvent(const SDL_DropEvent& event)
{
    Path file(event.file);

    SDL_free(event.file);
    SDL_RaiseWindow(sdl_video_device->window);

    return mmu.gamepak.load(file);
}
