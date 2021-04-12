#pragma once

#include <shell/array.h>

#include "base/int.h"
#include "base/opengl.h"
#include "base/sdl2.h"
#include "ppu/constants.h"

class VideoContext
{
public:
    using Scanline = shell::array<u32, kScreen.x>;

    ~VideoContext();

    void init();
    void raise();
    void fullscreen();
    void title(const std::string& title);

    void renderClear(u8 r, u8 g, u8 b);
    void renderFrame();
    void renderIcon(float top_offset);
    void swapWindow();
    void updateViewport();

    Scanline& scanline(uint line);

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

private:
    bool initWindow();
    bool initOpenGL();
    void initImgui();

    template<uint kTextureW, uint kTextureH>
    void renderTexture(GLuint texture, const void* data, bool preserve_ratio, GLfloat top_offset);

    GLuint main_texture;
    GLuint icon_texture;
    shell::array<Scanline, kScreen.y> buffer = {};
};

inline VideoContext video_ctx;
