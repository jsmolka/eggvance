#include "videocontext.h"

#include <shell/errors.h>
#include <shell/icon.h>

#include "base/bit.h"
#include "base/opengl.h"

VideoContext::~VideoContext()
{
    if (SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
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
    if (!initRenderer()) throw shell::Error("Cannot init renderer: {}", SDL_GetError());
    if (!initTexture())  throw shell::Error("Cannot init texture: {}", SDL_GetError());
    
    initImgui();
}

void VideoContext::raise()
{
    SDL_RaiseWindow(window);
}

void VideoContext::fullscreen()
{
    SDL_ShowCursor(SDL_ShowCursor(SDL_QUERY) ^ 0x1);
    SDL_SetWindowFullscreen(window, SDL_GetWindowFlags(window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void VideoContext::title(const std::string& title)
{
    SDL_SetWindowTitle(window, title.c_str());
}

void VideoContext::renderCopyTexture()
{
    SDL_UpdateTexture(texture, NULL, buffer.data(), sizeof(buffer.front()));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void VideoContext::renderPresent()
{
    SDL_RenderPresent(renderer);
}

void VideoContext::renderIcon()
{
    int w;
    int h;
    SDL_RenderGetLogicalSize(renderer, &w, &h);
    SDL_RenderSetLogicalSize(renderer, 18, 18);

    for (const auto& pixel : shell::icon::kPixels)
    {
        SDL_SetRenderDrawColor(
            renderer,
            pixel.r(),
            pixel.g(),
            pixel.b(),
            SDL_ALPHA_OPAQUE);

        SDL_RenderDrawPoint(
            renderer,
            pixel.x() + 1,
            pixel.y() + 1);
    }

    SDL_RenderSetLogicalSize(renderer, w, h);
}

shell::array<u32, kScreen.x>& VideoContext::scanline(uint line)
{
    return buffer[line];
}

void VideoContext::renderClear(u32 color)
{
    SDL_SetRenderDrawColor(
        renderer,
        bit::seq<16, 8>(color),
        bit::seq< 8, 8>(color),
        bit::seq< 0, 8>(color),
        SDL_ALPHA_OPAQUE);

    SDL_RenderClear(renderer);
}

bool VideoContext::initWindow()
{
    #ifdef IMGUI_IMPL_OPENGL_ES2
    glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    glsl_version = "#version 150";
    #elif SHELL_OS_MACOS
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
    glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    window = SDL_CreateWindow(
        "eggvance",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        2 * kScreen.x, 2 * kScreen.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window)
        SDL_SetWindowMinimumSize(window, kScreen.x, kScreen.y);;

    return window;
}

bool VideoContext::initOpenGL()
{
    context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, context);

    int ret = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);

    return ret;
}

bool VideoContext::initRenderer()
{
    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    SDL_RenderSetLogicalSize(renderer, kScreen.x, kScreen.y);

    return renderer;
}

bool VideoContext::initTexture()
{
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        kScreen.x, kScreen.y);

    return texture;
}

void VideoContext::initImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}
