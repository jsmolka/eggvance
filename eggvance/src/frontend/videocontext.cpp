#include "videocontext.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/imgui_impl_sdl.h>
#include <shell/errors.h>
#include <shell/icon.h>

#include "base/bit.h"
#include "base/config.h"

VideoContext::~VideoContext()
{
    if (SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

void VideoContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
        throw shell::Error("Cannot init video context: {}", SDL_GetError());

    if (!initWindow())   throw shell::Error("Cannot init window: {}", SDL_GetError());
    if (!initOpenGL())   throw shell::Error("Cannot init OpenGL");
    
    initImGui();
}

void VideoContext::title(const std::string& title)
{
    SDL_SetWindowTitle(window, title.c_str());
}

void VideoContext::renderClear(u8 r, u8 g, u8 b)
{
    glClearColor(
        GLfloat(r) / 255.0f,
        GLfloat(g) / 255.0f,
        GLfloat(b) / 255.0f, 1);

    glClear(GL_COLOR_BUFFER_BIT);
}

void VideoContext::renderIcon(GLfloat padding_top)
{
    shell::array<u32, 18, 18> icon = {};

    for (const auto& pixel : shell::icon::kPixels)
    {
        uint x = pixel.x() + 1;
        uint y = pixel.y() + 1;

        icon[y][x] = 0xFF00'0000 | (pixel.r() << 16) | (pixel.g() << 8) | pixel.b();
    }

    renderClear(62, 71, 80);
    renderTexture(icon_texture, 18, 18, icon.front().data(), true, padding_top);
}

void VideoContext::renderFrame()
{
    renderClear(0, 0, 0);
    renderTexture(frame_texture, kScreenW, kScreenH, framebuffer.front().data(), config.preserve_aspect_ratio, 0);
}

void VideoContext::swapWindow()
{
    SDL_GL_SwapWindow(window);
}

void VideoContext::updateViewport()
{
    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);
}

VideoContext::Scanline& VideoContext::scanline(uint line)
{
    return framebuffer[line];
}

bool VideoContext::initWindow()
{
    window = SDL_CreateWindow(
        "eggvance",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.frame_size * kScreenW,
        config.frame_size * kScreenH,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window)
        SDL_SetWindowMinimumSize(window, kScreenW, kScreenH);;

    return window;
}

bool VideoContext::initOpenGL()
{
    context = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glGenTextures(1, &icon_texture);
    glBindTexture(GL_TEXTURE_2D, icon_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &frame_texture);
    glBindTexture(GL_TEXTURE_2D, frame_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_GL_SetSwapInterval(0);

    return true;
}

void VideoContext::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL2_Init();
}

void VideoContext::renderTexture(GLuint texture, GLfloat texture_w, GLfloat texture_h, const void* data, bool preserve_ratio, GLfloat padding_top)
{
    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);

    GLfloat window_w = GLfloat(w);
    GLfloat window_h = GLfloat(h) - padding_top;
    GLfloat offset_x = 0;
    GLfloat offset_y = 0;

    if (preserve_ratio)
    {
        GLfloat window_ratio  = window_w  / window_h;
        GLfloat texture_ratio = texture_w / texture_h;

        GLfloat aspect_w = window_w;
        GLfloat aspect_h = window_h;

        if (texture_ratio > window_ratio)
            aspect_h = aspect_w / texture_ratio;
        else
            aspect_w = aspect_h * texture_ratio;

        offset_x = (window_w - aspect_w) * 0.5f;
        offset_y = (window_h - aspect_h) * 0.5f;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-1, -1, 0);
    glScalef(2.0f / GLfloat(w), 2.0f / GLfloat(h), 1.0);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(           offset_x, window_h - offset_y - padding_top);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(window_w - offset_x, window_h - offset_y - padding_top);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(window_w - offset_x,            offset_y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(           offset_x,            offset_y);
    glEnd();
}
