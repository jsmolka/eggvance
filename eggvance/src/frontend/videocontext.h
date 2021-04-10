#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <shell/array.h>

#include "base/int.h"
#include "base/sdl2.h"
#include "ppu/constants.h"

class VideoContext
{
public:
    ~VideoContext();

    void init();
    void raise();
    void fullscreen();
    void title(const std::string& title);

    void renderClear(u32 color);
    void renderCopyTexture();
    void renderPresent();
    void renderIcon();

    shell::array<u32, kScreen.x>& scanline(uint line);

    SDL_Window* window = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_GLContext context = nullptr;

private:
    bool initWindow();
    bool initOpenGL();
    bool initRenderer();
    bool initTexture();
    void initImgui();

    std::string glsl_version;
    shell::array<u32, kScreen.y, kScreen.x> buffer = {};
};

inline VideoContext video_ctx;
