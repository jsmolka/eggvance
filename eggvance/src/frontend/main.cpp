#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/imgui_impl_sdl.h>
#include <nfd/nfd.h>
#include <shell/options.h>
#include <shell/utility.h>

#include "audiocontext.h"
#include "framecounter.h"
#include "frameratelimiter.h"
#include "inputcontext.h"
#include "videocontext.h"
#include "apu/apu.h"
#include "arm/arm.h"
#include "base/config.h"
#include "base/opengl.h"
#include "dma/dma.h"
#include "gamepak/gamepak.h"
#include "keypad/keypad.h"
#include "ppu/color.h"
#include "ppu/ppu.h"
#include "scheduler/scheduler.h"
#include "sio/sio.h"
#include "timer/timer.h"

bool running = true;
bool paused  = false;
FrameCounter counter;
FrameRateLimiter limiter(kRefreshRate);

void updateTitle()
{
    const auto title = shell::format(
        gamepak.rom.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        gamepak.rom.title);

    video_ctx.title(title);
}

void updateTitle(double fps)
{
    const auto title = shell::format(
        gamepak.rom.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        gamepak.rom.title, fps);

    video_ctx.title(title);
}

void reset()
{
    gamepak.save->reset();
    gamepak.gpio->reset();

    shell::reconstruct(apu);
    shell::reconstruct(arm);
    shell::reconstruct(dma);
    shell::reconstruct(ppu);
    shell::reconstruct(keypad);
    shell::reconstruct(sio);
    shell::reconstruct(scheduler);
    shell::reconstruct(timer);

    apu.init();
    arm.init();
    ppu.init();

    updateTitle();

    paused = false;
}

void loadRomFile(const fs::path& file)
{
    audio_ctx.pause();

    auto iter = std::find(config.recent.begin(), config.recent.end(), file);
    if ( iter != config.recent.end())
        config.recent.erase(iter);

    config.recent.push_back(file);

    if (config.recent.size() == 11)
        config.recent.erase(config.recent.begin());
    
    gamepak.init(config.recent.back(), fs::path());
    reset();

    audio_ctx.unpause();
}

void openRomFile()
{
    nfdchar_t* file = NULL;
    if (NFD_OpenDialog("gba", NULL, &file) == NFD_OKAY)
    {
        loadRomFile(fs::u8path(file));
        free(file);
    }
    
    limiter.queueReset();
    counter.queueReset();
}

void loadSavFile(const fs::path& file)
{
    audio_ctx.pause();
    
    gamepak.init(fs::path(), file);
    reset();
    
    audio_ctx.unpause();
}

void openSavFile()
{
    nfdchar_t* file = NULL;
    if (NFD_OpenDialog("sav", NULL, &file) == NFD_OKAY)
    {
        loadSavFile(fs::u8path(file));
        free(file);
    }

    limiter.queueReset();
    counter.queueReset();
}

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.mod & KMOD_CTRL)
            {
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_O:
                    openRomFile();
                    break;

                case SDL_SCANCODE_R:
                    if (!gamepak.rom.empty())
                        reset();
                    break;

                case SDL_SCANCODE_P:
                    if (!gamepak.rom.empty())
                        paused = !paused;
                    break;
                }
            }
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.handleDeviceEvent(event.cdevice);
            break;
        }
    }

    video_ctx.updateViewport();
}

float runUi()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(video_ctx.window);

    ImGui::NewFrame();
    ImGui::BeginMainMenuBar();

    float height = ImGui::GetWindowHeight() / 2.0f;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open ROM", "Ctrl+O"))
            openRomFile();

        if (ImGui::MenuItem("Open save", nullptr, false, !gamepak.rom.empty()))
            openSavFile();

        if (ImGui::BeginMenu("Recent", !config.recent.empty()))
        {
            for (const auto& file : shell::reversed(config.recent))
            {
                if (ImGui::MenuItem(file.string().c_str()))
                    loadRomFile(fs::path(file));
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
            running = false;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Emulation"))
    {
        if (ImGui::MenuItem("Reset", "Ctrl+R", nullptr, !gamepak.rom.empty()))
            reset();

        if (ImGui::MenuItem("Pause", "Ctrl+P", paused, !gamepak.rom.empty()))
            paused = !paused;

        ImGui::Separator();

        static bool fast_forward = false;
        static bool slow_motion  = false;

        ImGui::MenuItem("Fast forward", "Ctrl+Shift", &fast_forward, !slow_motion);

        if (ImGui::BeginMenu("Fast forward speed", !slow_motion))
        {
            ImGui::MenuItem("Unbound");
            ImGui::Separator();
            ImGui::MenuItem("2x");
            ImGui::MenuItem("4x");
            ImGui::MenuItem("6x");

            ImGui::EndMenu();
        }

        ImGui::MenuItem("Slow motion", "Ctrl+Space", &slow_motion, !fast_forward);

        if (ImGui::BeginMenu("Slow motion speed", !fast_forward))
        {
            ImGui::MenuItem("0.2x");
            ImGui::MenuItem("0.4x");
            ImGui::MenuItem("0.6x");

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Audio/Video"))
    {
        if (ImGui::BeginMenu("Frame size"))
        {
            int w;
            int h;
            SDL_GetWindowSize(video_ctx.window, &w, &h);

            for (uint scale = 1; scale <= 8; ++scale)
            {
                std::string text = shell::format("{}x", scale);

                int window_w = 240 * scale;
                int window_h = 160 * scale;

                if (ImGui::MenuItem(text.c_str(), nullptr, w == window_w && h == window_h))
                {
                    SDL_SetWindowFullscreen(video_ctx.window, ~SDL_WINDOW_FULLSCREEN_DESKTOP);
                    SDL_SetWindowSize(video_ctx.window, window_w, window_h);
                }
            }

            ImGui::Separator();

            uint fullscreen = SDL_GetWindowFlags(video_ctx.window) & SDL_WINDOW_FULLSCREEN_DESKTOP;

            if (ImGui::MenuItem("Fullscreen", "Ctrl+F", fullscreen))
            {
                SDL_SetWindowFullscreen(video_ctx.window, fullscreen ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
            }

            ImGui::EndMenu();
        }

        ImGui::MenuItem("Color correct", nullptr, true);
        ImGui::MenuItem("Preserve aspect ratio", nullptr, true);

        ImGui::Separator();

        if (ImGui::BeginMenu("Volume"))
        {
            static float volume = 0.5;
            ImGui::SliderFloat("", &volume, 0.0f, 1.0f);

            ImGui::EndMenu();
        }

        ImGui::MenuItem("Mute", "Ctrl+M", false);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    ImGui::Render();

    return height;
}

void renderUi()
{
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

#if SHELL_OS_WINDOWS
int eventFilter(void*, SDL_Event* event)
{
    if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED)
    {
        video_ctx.updateViewport();

        if (gamepak.rom.size() == 0)
        {
            float height = runUi();
            video_ctx.renderIcon(height);
            renderUi();
        }
        else
            video_ctx.renderFrame();

        video_ctx.swapWindow();

        return 0;
    }
    else if (event->type == SDL_SYSWMEVENT && event->syswm.msg->msg.win.msg == WM_EXITSIZEMOVE)
    {
        limiter.queueReset();
        counter.queueReset();
        return 0;
    }
    return 1;
}
#endif

void init(int argc, char* argv[])
{
    using namespace shell;

    Options options("eggvance");
    options.add({ "-c,--config", "config file", "file" }, Options::value<fs::path>("eggvance.ini"));
    options.add({ "-s,--save",   "save file",   "file" }, Options::value<fs::path>()->optional());
    options.add({       "rom",   "ROM file"            }, Options::value<fs::path>()->positional()->optional());

    OptionsResult result;
    try
    {
        result = options.parse(argc, argv);
    }
    catch (const ParseError& error)
    {
        throw shell::Error("Cannot parse command line: {}", error.what());
    }

    const auto cfg = result.find<fs::path>("--config");
    const auto sav = result.find<fs::path>("--save");
    const auto gba = result.find<fs::path>("rom");

    config.init(fs::absolute(*cfg));

    audio_ctx.init();
    input_ctx.init();
    video_ctx.init();

    Bios::init(config.bios_file);
    Color::init(config.lcd_color);

    #if SHELL_OS_WINDOWS
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    SDL_SetEventFilter(eventFilter, NULL);
    #endif

    gamepak.init(
        gba.value_or(fs::path()),
        sav.value_or(fs::path()));
}

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);

        while (running && gamepak.rom.size() == 0)
        {
            handleEvents();

            float height = runUi();
            video_ctx.renderIcon(height);
            renderUi();
            video_ctx.swapWindow();

            SDL_Delay(16);
        }

        if (!running)
            return 0;

        reset();

        counter.reset();

        audio_ctx.unpause();

        while (running)
        {
            limiter.run([]() 
            {
                handleEvents();

                if (!paused)
                {
                    constexpr auto kPixelsHor   = 240 + 68;
                    constexpr auto kPixelsVer   = 160 + 68;
                    constexpr auto kPixelCycles = 4;
                    constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

                    keypad.update();
                    arm.run(kFrameCycles);
                }
                else
                {
                    video_ctx.renderFrame();
                }

                runUi();
                renderUi();

                video_ctx.swapWindow();
            });

            if (paused)
                counter.reset();
            else if (const auto fps = (++counter).fps())
                updateTitle(*fps);
        }

        audio_ctx.pause();
    }
    catch (const std::exception& ex)
    {
        showMessageBox("Exception", ex.what());
    }
    return 0;
}
