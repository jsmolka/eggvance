#pragma once

#include <string>

#include "base/constants.h"
#include "base/sdl2.h"

// Todo: renderCopyBuffer
// Todo: renderPresent

class VideoContext
{
public:
    friend class Context;

    void present();
    void fullscreen();
    void setWindowTitle(const std::string& title);

    void renderIcon();
    void clear(uint color);  // Todo: renderClear, u32

    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;

    u32 buffer[kScreenH * kScreenW];

private:
    void init();
    void deinit();

    bool initWindow();
    bool initRenderer();
    bool initTexture();
};
