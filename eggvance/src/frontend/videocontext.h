#pragma once

#include <shell/array.h>

#include "base/int.h"
#include "base/opengl.h"
#include "base/sdl2.h"
#include "ppu/constants.h"

class VideoContext
{
public:
    // Todo: scanline class

    ~VideoContext();

    void init();
    void raise();
    void fullscreen();
    void title(const std::string& title);

    void renderPresent();
    void renderIcon();
    void swapWindow();

    shell::array<u32, kScreen.x>& scanline(uint line);

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

private:
    bool initWindow();
    bool initOpenGL();
    void initImgui();

    GLuint main_texture;
    GLuint idle_texture;
    shell::array<u32, kScreen.y, kScreen.x> buffer = {};
};

inline VideoContext video_ctx;
