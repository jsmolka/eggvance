#pragma once

#include <string>

#include "base/constants.h"
#include "base/sdl2.h"

class VideoContext
{
public:
    friend class Context;

    void fullscreen();
    void setTitle(const std::string& title);

    void renderClear(u32 color);
    void renderCopyBuffer();
    void renderPresent();
    void renderIcon();

    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    
    // Todo: use LockTexture and UnlockTexture, remove entirely
    // https://wiki.libsdl.org/SDL_UpdateTexture
    u32 buffer[kScreenH * kScreenW];

private:
    void init();
    void deinit();

    bool initWindow();
    bool initRenderer();
    bool initTexture();
};
