#include "sdlvideodevice.h"

#include <exception>

SDLVideoDevice::~SDLVideoDevice()
{
    deinit();
}

void SDLVideoDevice::init()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
        throw std::runtime_error("Cannot init video device");

    if (!createWindow())
        throw std::runtime_error("Cannot create window");

    if (!createRenderer())
        throw std::runtime_error("Cannot create renderer");

    if (!createTexture())
        throw std::runtime_error("Cannot create texture");
}

void SDLVideoDevice::deinit()
{
    if (SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

void SDLVideoDevice::present()
{
    SDL_UpdateTexture(
        texture, nullptr, buffer,
        sizeof(u32) * 240
    );
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void SDLVideoDevice::fullscreen()
{
    u32 flag = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_SetWindowFullscreen(window, flag ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_ShowCursor(1 ^ SDL_ShowCursor(SDL_QUERY));
}

bool SDLVideoDevice::createWindow()
{
    return window = SDL_CreateWindow(
        "eggvance",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        2 * SCREEN_W, 2 * SCREEN_H,
        SDL_WINDOW_RESIZABLE
    );
}

bool SDLVideoDevice::createRenderer()
{
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );
    SDL_RenderSetLogicalSize(renderer, 240, 160);;

    return renderer;
}

bool SDLVideoDevice::createTexture()
{
    return texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_W, SCREEN_H
    );
}
