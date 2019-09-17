#pragma once

#include <array>
#include <SDL2/SDL.h>

#include "common/integer.h"
#include "constants.h"

struct Backend
{
    Backend()
    {
        SDL_InitSubSystem(SDL_INIT_VIDEO);

        window = SDL_CreateWindow(
            "eggvance",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            2 * WIDTH, 2 * HEIGHT,
            SDL_WINDOW_RESIZABLE
        );

        renderer = SDL_CreateRenderer(
            window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
        );
        SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_BGR555,
            SDL_TEXTUREACCESS_STREAMING,
            WIDTH, HEIGHT
        );
    }

    ~Backend()
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    inline void present()
    {
        SDL_UpdateTexture(
            texture, nullptr,
            buffer.data(),
            sizeof(u16) * WIDTH
        );
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    inline void fullscreen()
    {
        u32 flag = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
        SDL_SetWindowFullscreen(window, flag ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(1 ^ SDL_ShowCursor(SDL_QUERY));
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<u16, WIDTH * HEIGHT> buffer;
};