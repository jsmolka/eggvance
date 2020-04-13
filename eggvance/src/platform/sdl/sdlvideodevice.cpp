#include "sdlvideodevice.h"

#include <stdexcept>

#include "icon.h"

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
    SDL_UpdateTexture(texture, nullptr, buffer, sizeof(u32) * SCREEN_W);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void SDLVideoDevice::fullscreen()
{
    SDL_ShowCursor(SDL_ShowCursor(SDL_QUERY) ^ 0x1);
    SDL_SetWindowFullscreen(window, SDL_GetWindowFlags(window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void SDLVideoDevice::setWindowTitle(const std::string& title)
{
    SDL_SetWindowTitle(window, title.c_str());
}

void SDLVideoDevice::renderIcon()
{
    int w;
    int h;
    SDL_RenderGetLogicalSize(renderer, &w, &h);
    SDL_RenderSetLogicalSize(renderer, 18, 18);

    for (const auto& pixel : egg::icon::pixels)
    {
        SDL_SetRenderDrawColor(
            renderer,
            pixel.r(),
            pixel.g(),
            pixel.b(),
            SDL_ALPHA_OPAQUE
        );
        SDL_RenderDrawPoint(
            renderer,
            pixel.x() + 1,
            pixel.y() + 1
        );
    }

    SDL_RenderSetLogicalSize(renderer, w, h);
}

void SDLVideoDevice::clear(uint color)
{
    SDL_SetRenderDrawColor(
        renderer,
        (color >> 16) & 0xFF,
        (color >>  8) & 0xFF,
        (color >>  0) & 0xFF,
        SDL_ALPHA_OPAQUE
    );
    SDL_RenderClear(renderer);
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
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);

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
