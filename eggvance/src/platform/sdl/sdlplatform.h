#pragma once

#include "sdlaudiodevice.h"
#include "sdlinputdevice.h"
#include "sdlvideodevice.h"
#include "platform/platform.h"

class SDLPlatform : public Platform
{
protected:
    void initHookBeg() override;
    void initHookEnd() override;
    void mainHookBeg() override;
    void mainHookEnd() override;

    void processEvents() override;

private:
    bool dropAwait();
    bool dropEvent(const SDL_DropEvent& event);

    std::shared_ptr<SDLAudioDevice> sdl_audio_device;
    std::shared_ptr<SDLInputDevice> sdl_input_device;
    std::shared_ptr<SDLVideoDevice> sdl_video_device;

    struct Shortcuts
    {
        ShortcutConfig<SDL_Scancode> keyboard;
        ShortcutConfig<SDL_GameControllerButton> controller;
    } shortcuts;
};
