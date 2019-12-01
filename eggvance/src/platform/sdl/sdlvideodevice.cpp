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
    SDL_UpdateTexture(
        texture, nullptr, buffer,
        sizeof(u32) * SCREEN_W
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

void SDLVideoDevice::renderIcon()
{
    constexpr int scale = 9;

    SDL_Rect rect = { 0, 0, SCREEN_W, SCREEN_H };
    SDL_SetRenderDrawColor(renderer, 38, 40, 43, 1);
    SDL_RenderFillRect(renderer, &rect);

    rect.w = scale;
    rect.h = scale;
    for (const auto& pixel : icon)
    {
        rect.x = scale * pixel.x + 48;
        rect.y = scale * pixel.y + 8;
        SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, 1);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void SDLVideoDevice::raiseWindow()
{
    SDL_RaiseWindow(window);
}

void SDLVideoDevice::setWindowTitle(const std::string& title)
{
    SDL_SetWindowTitle(window, title.c_str());
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
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);;

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
