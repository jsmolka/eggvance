#pragma once

#include <array>
#include <SDL2/SDL.h>

#include "common/integer.h"
#include "constants.h"
#include "icon.h"

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
            window, 
            -1, 
            SDL_RENDERER_ACCELERATED
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

    inline void copyTexture()
    {
        SDL_UpdateTexture(
            texture, nullptr,
            buffer.data(),
            sizeof(u16) * WIDTH
        );
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    }

    inline void preset()
    {
        SDL_RenderPresent(renderer);
    }

    inline void fullscreen()
    {
        u32 flag = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
        SDL_SetWindowFullscreen(window, flag ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(1 ^ SDL_ShowCursor(SDL_QUERY));
    }

    inline void renderIcon()
    {
        SDL_Rect rect = { 0, 0, WIDTH, HEIGHT };
        SDL_SetRenderDrawColor(renderer, 48, 48, 48, 1);
        SDL_RenderFillRect(renderer, &rect);

        int scale = 9;
        rect.w = scale;
        rect.h = scale;
        for (const IconPixel& pixel : icon)
        {
            rect.x = scale * pixel.x + 48;
            rect.y = scale * pixel.y + 8;
            SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, 1);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<u16, WIDTH * HEIGHT> buffer;
};