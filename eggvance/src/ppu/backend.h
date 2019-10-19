#pragma once

#include <array>
#include <SDL2/SDL.h>

#include "common/integer.h"

struct Backend
{
    Backend();
    ~Backend();

    void present();
    void fullscreen();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<u32, 240 * 160> buffer;
};
