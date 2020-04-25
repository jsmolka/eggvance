#pragma once

#include "base/defines.h"
#include "platform/videodevice.h"

#if COMPILER_MSVC || COMPILER_EMSCRIPTEN
#include <SDL2/SDL.h>
#else
#include "SDL.h"
#endif

class SDLVideoDevice : public VideoDevice
{
public:
    ~SDLVideoDevice();

    void init() override;
    void deinit() override;
    void present() override;
    void fullscreen() override;
    void setWindowTitle(const std::string& title) override;

    void renderIcon();
    void clear(uint color);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

private:
    bool createWindow();
    bool createRenderer();
    bool createTexture();
};
