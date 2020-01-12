#pragma once

#include <string>

#ifdef _MSC_VER
#include <SDL2/SDL.h>
#else
#include "SDL.h"
#endif

#include "platform/videodevice.h"

class SDLVideoDevice : public VideoDevice
{
public:
    ~SDLVideoDevice();

    void init() override;
    void deinit() override;
    void present() override;
    void fullscreen() override;
    void title(const std::string& title) override;

    void renderIcon();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

private:
    bool createWindow();
    bool createRenderer();
    bool createTexture();
};
