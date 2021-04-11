#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/imgui_impl_sdl.h>
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
bool changed = false;
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
}

void handleDropEvent(const SDL_DropEvent& event)
{
    const auto file = fs::u8path(event.file);

    SDL_free(event.file);

    audio_ctx.pause();

    if (file.extension() == ".gba")
    {
        gamepak.init(file, fs::path());
        reset();
    }
    if (file.extension() == ".sav")
    {
        gamepak.init(fs::path(), file);
        reset();
    }

    audio_ctx.unpause();
    video_ctx.raise();
}

template<typename Input>
void handleInputEvent(const Shortcuts<Input>& shortcuts, Input input)
{
    if      (input == shortcuts.reset)          reset();
    else if (input == shortcuts.pause)          paused ^= true;
    else if (input == shortcuts.fullscreen)     video_ctx.fullscreen();
    else if (input == shortcuts.volume_up)      config.volume = std::clamp(config.volume + config.volume_step, 0.0, 1.0);
    else if (input == shortcuts.volume_down)    config.volume = std::clamp(config.volume - config.volume_step, 0.0, 1.0);
    else if (input == shortcuts.speed_hardware) limiter = FrameRateLimiter(kRefreshRate);
    else if (input == shortcuts.speed_2x)       limiter = FrameRateLimiter(kRefreshRate * 2);
    else if (input == shortcuts.speed_4x)       limiter = FrameRateLimiter(kRefreshRate * 4);
    else if (input == shortcuts.speed_6x)       limiter = FrameRateLimiter(kRefreshRate * 6);
    else if (input == shortcuts.speed_8x)       limiter = FrameRateLimiter(kRefreshRate * 8);
    else if (input == shortcuts.speed_unbound)  limiter = FrameRateLimiter(1'000'000);
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
            handleInputEvent(
                config.shortcuts.keyboard,
                event.key.keysym.scancode);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            handleInputEvent(
                config.shortcuts.controller,
                SDL_GameControllerButton(event.cbutton.button));
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.handleDeviceEvent(event.cdevice);
            break;

        case SDL_DROPFILE:
            handleDropEvent(event.drop);
            break;
        }
    }
}

#if SHELL_OS_WINDOWS
int eventFilter(void*, SDL_Event* event)
{
    if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED)
    {
        if (gamepak.rom.size() == 0)
        {
            //video_ctx.renderClear(0xFF3E'4750);
            video_ctx.renderIcon();
            video_ctx.renderPresent();
        }
        else
        {
            video_ctx.renderPresent();
        }
        return 0;
    }
    else if (event->type == SDL_SYSWMEVENT && event->syswm.msg->msg.win.msg == WM_EXITSIZEMOVE)
    {
        changed = true;
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

            //video_ctx.renderClear(0xFF3E'4750);
            video_ctx.renderIcon();
            video_ctx.renderPresent();

            SDL_Delay(16);
        }

        if (!running)
            return 0;

        reset();

        counter.reset();

        audio_ctx.unpause();

        while (running)
        {
            handleEvents();

            limiter.run([]() 
            {
                handleEvents();

                if (paused)
                    return;

                constexpr auto kPixelsHor   = 240 + 68;
                constexpr auto kPixelsVer   = 160 + 68;
                constexpr auto kPixelCycles = 4;
                constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

                keypad.update();
                arm.run(kFrameCycles);
                ppu.present();
            });

            if (changed)
            {
                counter.reset();
                limiter.reset();
                changed = false;
            }
            else if (paused)
            {
                counter.reset();
            }
            else if (const auto fps = (++counter).fps())
            {
                updateTitle(*fps);
            }

            {
                ImGui_ImplOpenGL2_NewFrame();
                ImGui_ImplSDL2_NewFrame(video_ctx.window);
    
                int w;
                int h;
                SDL_GetWindowSize(video_ctx.window, &w, &h);

                ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);

                ImGui::NewFrame();
                {
                    ImGui::BeginMainMenuBar();
                    {
                        if (ImGui::BeginMenu("File"))
                        {
                            ImGui::MenuItem("Open ROM", "Ctrl+O");
                            ImGui::MenuItem("Open save");  // Disable if no rom

                            if (ImGui::BeginMenu("Recent"))
                            {
                                ImGui::MenuItem("recent0.gba");
                                ImGui::MenuItem("recent1.gba");
                                ImGui::MenuItem("recent2.gba");
                                ImGui::EndMenu();
                            }

                            ImGui::Separator();
                            ImGui::MenuItem("Exit");

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Emulation"))
                        {
                            ImGui::MenuItem("Reset", "Ctrl+R");
                            ImGui::MenuItem("Pause", "Ctrl+P");

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
                    }
                    ImGui::EndMainMenuBar();
                }
                ImGui::Render();
                ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
            }
            video_ctx.swapWindow();
        }

        audio_ctx.pause();
    }
    catch (const std::exception& ex)
    {
        showMessageBox("Exception", ex.what());
    }
    return 0;
}
