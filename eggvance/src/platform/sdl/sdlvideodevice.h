#pragma once

#include <SDL2/SDL.h>

#include "devices/videodevice.h"

class SDLVideoDevice : public VideoDevice
{
public:
    ~SDLVideoDevice();

    void init() override;
    void deinit() override;
    void present() override;
    void fullscreen() override;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

private:
    bool createWindow();
    bool createRenderer();
    bool createTexture();
};