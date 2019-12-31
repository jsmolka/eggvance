#pragma once

#include <string>

#include "platform.h"
#include "devices/videodevice.h"

class SDLVideoDevice : public VideoDevice
{
public:
    ~SDLVideoDevice();

    void init() override;
    void deinit() override;
    void present() override;
    void fullscreen() override;

    void renderIcon();
    void raiseWindow();
    void setWindowTitle(const std::string& title);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

private:
    bool createWindow();
    bool createRenderer();
    bool createTexture();
};
