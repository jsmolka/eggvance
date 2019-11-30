#include "window.h"

#include "common/message.h"

Window::~Window()
{
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
        return;

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool Window::init()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
    {
        showMessage("Cannot init video backend.");
        return false;
    }
    if (!createWindow())
    {
        showMessage("Cannot create window.");
        return false;
    }
    if (!createRenderer())
    {
        showMessage("Cannot create renderer.");
        return false;
    }
    if (!createTexture())
    {
        showMessage("Cannot create texture.");
        return false;
    }
    return true;
}

void Window::present()
{
    SDL_UpdateTexture(
        texture, nullptr,
        buffer.data(),
        sizeof(buffer[0]) * 240
    );
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Window::fullscreen()
{
    u32 flag = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_SetWindowFullscreen(window, flag ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_ShowCursor(1 ^ SDL_ShowCursor(SDL_QUERY));
}

bool Window::createWindow()
{
    return window = SDL_CreateWindow(
        "eggvance",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        2 * 240, 2 * 160,
        SDL_WINDOW_RESIZABLE
    );
}

bool Window::createRenderer()
{
    renderer = SDL_CreateRenderer(
        window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );
    SDL_RenderSetLogicalSize(renderer, 240, 160);;

    return renderer;
}

bool Window::createTexture()
{
    return texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        240, 160
    );
}
