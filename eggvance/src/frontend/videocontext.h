#pragma once

#include <array>
#include <string>

#include "base/int.h"
#include "base/sdl2.h"
#include "ppu/constants.h"

class VideoContext
{
public:
    ~VideoContext();

    void init();

    void raise();
    void fullscreen();
    void title(const std::string& title);

    void renderClear(u32 color);
    void renderCopyTexture();
    void renderPresent();
    void renderIcon();

    u32* scanline(uint line);

private:
    void deinit();

    bool initWindow();
    bool initRenderer();
    bool initTexture();

    SDL_Window* window = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::array<u32, kScreen.x * kScreen.y> buffer;
};

inline VideoContext video_ctx;
