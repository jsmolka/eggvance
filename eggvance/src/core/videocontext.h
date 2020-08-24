#pragma once

#include <array>
#include <string>

#include "base/constants.h"
#include "base/int.h"
#include "base/sdl.h"

class VideoContext
{
public:
    friend class Context;

    void raise();
    void fullscreen();
    void title(const std::string& title);

    void renderClear(u32 color);
    void renderCopyTexture();
    void renderPresent();
    void renderIcon();

    u32* scanline(uint line);

private:
    void init();
    void deinit();

    bool initWindow();
    bool initRenderer();
    bool initTexture();

    SDL_Window* window = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::array<u32, kScreenW * kScreenH> buffer = { 0 };
};
