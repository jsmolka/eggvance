#pragma once

#include <shell/array.h>

#include "opengl.h"
#include "sdl2.h"
#include "base/constants.h"
#include "base/int.h"

class VideoContext
{
public:
    using Scanline = shell::array<u32, kScreenW>;

    ~VideoContext();

    template<typename... Args>
    void showMessageBox(const std::string& title, const std::string& format, Args&&... args)
    {
        std::string message = shell::format(format, std::forward<Args>(args)...);

        SDL_ShowSimpleMessageBox(0, title.c_str(), message.c_str(), window);
    }

    void init();
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
