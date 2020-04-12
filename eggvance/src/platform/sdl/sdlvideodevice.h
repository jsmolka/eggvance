#pragma once

#include <string>

#if defined(_MSC_VER) || defined(__EMSCRIPTEN__)
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

    void fullscreen();
    void renderIcon();
    void clear(uint color);
    void title(const std::string& title);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

private:
    bool createWindow();
    bool createRenderer();
    bool createTexture();
};
