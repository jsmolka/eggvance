#pragma once

#include <array>
#include <SDL2/SDL.h>

#include "common/integer.h"
#include "common/constants.h"

class Window
{
public:
    ~Window();

    bool init();
    void present();
    void fullscreen();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<u32, SCREEN_W * SCREEN_H> buffer;

private:
    bool createWindow();
    bool createRenderer();
    bool createTexture();
};
