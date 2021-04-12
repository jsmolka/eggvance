#pragma once

#include <shell/array.h>

#include "base/constants.h"
#include "base/int.h"
#include "base/opengl.h"
#include "base/sdl2.h"

class VideoContext
{
public:
    using Scanline = shell::array<u32, kScreenW>;

    ~VideoContext();

    void init();
    void raise();
    void fullscreen();
    void title(const std::string& title);

    void renderClear(u8 r, u8 g, u8 b);
    void renderIcon(GLfloat padding_top);
    void renderFrame();
    void swapWindow();
    void updateViewport();

    Scanline& scanline(uint line);

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

private:
    bool initWindow();
    bool initOpenGL();
    void initImGui();

    void renderTexture(GLuint texture, GLfloat texture_w, GLfloat texture_h, const void* data, bool preserve_ratio, GLfloat padding_top = 0);

    GLuint icon_texture = 0;
    GLuint frame_texture = 0;
    shell::array<Scanline, kScreenH> framebuffer = {};
};

inline VideoContext video_ctx;
