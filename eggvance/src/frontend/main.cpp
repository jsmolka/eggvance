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

enum class UiState
{
    Quit,
    Menu,
    Emulate,
    Pause
} state;

FrameCounter counter;
FrameRateLimiter limiter;

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

    state = UiState::Emulate;
}

void loadRomFile(const fs::path& file)
{
    audio_ctx.pause();

    config.recent.push(file);
    
    gamepak.init(file, fs::path());
    reset();

    audio_ctx.unpause();

    limiter.queueReset();
    counter.queueReset();
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
   
    limiter.queueReset();
    counter.queueReset();
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

void doEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            state = UiState::Quit;
            return;

        case SDL_KEYDOWN:
            if (event.key.keysym.mod & KMOD_CTRL)
            {
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_O:
                    openRomFile();
                    break;

                case SDL_SCANCODE_R:
                    if (state != UiState::Menu)
                        reset();
                    break;

                case SDL_SCANCODE_P:
                    switch (state)
                    {
                    case UiState::Emulate:
                        state = UiState::Pause;
                        break;

                    case UiState::Pause:
                        state = UiState::Emulate;
                        break;
                    }
                    break;

                case SDL_SCANCODE_F:
                    SDL_SetWindowFullscreen(video_ctx.window, SDL_GetWindowFlags(video_ctx.window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
                    break;

                case SDL_SCANCODE_M:
                    config.mute ^= true;
                    break;

                case SDL_SCANCODE_LSHIFT:
                case SDL_SCANCODE_RSHIFT:
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate);
                    else
                        limiter.setFps(kRefreshRate * double(config.fast_forward));
                    break;

                case SDL_SCANCODE_1:
                    config.fast_forward = 1'000'000;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(1'000'000));
                    break;

                case SDL_SCANCODE_2:
                    config.fast_forward = 2;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(2));
                    break;

                case SDL_SCANCODE_3:
                    config.fast_forward = 3;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(3));
                    break;

                case SDL_SCANCODE_4:
                    config.fast_forward = 4;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(4));
                    break;

                case SDL_SCANCODE_5:
                    config.fast_forward = 5;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(5));
                    break;

                case SDL_SCANCODE_6:
                    config.fast_forward = 6;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(6));
                    break;

                case SDL_SCANCODE_7:
                    config.fast_forward = 7;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(7));
                    break;

                case SDL_SCANCODE_8:
                    config.fast_forward = 8;
                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(8));
                    break;

                }
            }
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.doDeviceEvent(event.cdevice);
            break;
        }
    }
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

        if (ImGui::BeginMenu("Recent", config.recent.hasFiles()))
        {
            for (const auto& file : config.recent)
            {
                if (file.empty())
                    break;

                if (ImGui::MenuItem(file.string().c_str()))
                    loadRomFile(fs::path(file));
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
            state = UiState::Quit;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Emulation"))
    {
        if (ImGui::MenuItem("Reset", "Ctrl+R", nullptr, !gamepak.rom.empty()))
            reset();

        if (ImGui::MenuItem("Pause", "Ctrl+P", state == UiState::Pause, state != UiState::Menu))
        {
            switch (state)
            {
            case UiState::Emulate:
                state = UiState::Pause;
                break;

            case UiState::Pause:
                state = UiState::Emulate;
                break;
            }
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Fast forward", "Ctrl+Shift", limiter.isFastForward()))
        {
            if (limiter.isFastForward())
                limiter.setFps(kRefreshRate);
            else
                limiter.setFps(kRefreshRate * double(config.fast_forward));
        }

        if (ImGui::BeginMenu("Fast forward speed"))
        {
            uint times = 1'000'000;

            if (ImGui::MenuItem("Unbound", "Ctrl+1", config.fast_forward == times))
            {
                config.fast_forward = times;

                if (limiter.isFastForward())
                    limiter.setFps(kRefreshRate * double(times));
            }

            ImGui::Separator();

            for (times = 2; times <= 8; ++times)
            {
                std::string text = shell::format("{}x", times);
                std::string shortcut = shell::format("Ctrl+{}", times);

                if (ImGui::MenuItem(text.c_str(), shortcut.c_str(), config.fast_forward == times))
                {
                    config.fast_forward = times;

                    if (limiter.isFastForward())
                        limiter.setFps(kRefreshRate * double(times));
                }
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Save type"))
        {
            static constexpr std::pair<std::string_view, Save::Type> kSaveTypes[] =
            {
                { "Detect",     Save::Type::Detect    },
                { "None",       Save::Type::None      },
                { "SRAM",       Save::Type::Sram      },
                { "EEPROM",     Save::Type::Eeprom    },
                { "Flash 512K", Save::Type::Flash512  },
                { "Flash 1M",   Save::Type::Flash1024 }
            };

            for (const auto& [text, type] : kSaveTypes)
            {
                if (ImGui::MenuItem(text.data(), nullptr, config.save_type == type))
                    config.save_type = type;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GPIO type"))
        {
            static constexpr std::pair<std::string_view, Gpio::Type> kGpioTypes[] =
            {
                { "Detect", Gpio::Type::Detect },
                { "None",   Gpio::Type::None   },
                { "RTC",    Gpio::Type::Rtc    }
            };

            for (const auto& [text, type] : kGpioTypes)
            {
                if (ImGui::MenuItem(text.data(), nullptr, config.gpio_type == type))
                    config.gpio_type = type;
            }

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

                int window_w = kScreenW * scale;
                int window_h = kScreenH * scale;

                if (ImGui::MenuItem(text.c_str(), nullptr, w == window_w && h == window_h))
                {
                    config.frame_size = scale;
                    SDL_SetWindowFullscreen(video_ctx.window, ~SDL_WINDOW_FULLSCREEN_DESKTOP);
                    SDL_SetWindowSize(video_ctx.window, window_w, window_h);
                    video_ctx.updateViewport();
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

        if (ImGui::MenuItem("Color correct", nullptr, config.color_correct))
        {
            config.color_correct ^= true;
            Color::init(config.color_correct);
        }

        if (ImGui::MenuItem("Preserve aspect ratio", nullptr, config.preserve_aspect_ratio))
            config.preserve_aspect_ratio ^= true;

        ImGui::Separator();

        if (ImGui::BeginMenu("Volume"))
        {
            ImGui::SliderFloat("", &config.volume, 0.0f, 1.0f);

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Mute", "Ctrl+M", config.mute))
            config.mute ^= true;

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

void emulate()
{
    constexpr auto kPixelsHor   = 240 + 68;
    constexpr auto kPixelsVer   = 160 + 68;
    constexpr auto kPixelCycles = 4;
    constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

    if (state == UiState::Emulate)
    {
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
}

void menu()
{
    auto padding_top = runUi();
    video_ctx.renderIcon(padding_top);
    renderUi();
    video_ctx.swapWindow();
}

int eventFilter(void*, SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            video_ctx.updateViewport();

            switch (state)
            {
            case UiState::Pause:
            case UiState::Emulate:
                video_ctx.renderFrame();
                video_ctx.swapWindow();
                break;

            case UiState::Menu:
                video_ctx.renderIcon(0);
                video_ctx.swapWindow();
                break;
            }
            return 0;
        }
        break;
    
    #if SHELL_OS_WINDOWS
    case SDL_SYSWMEVENT:
        if (event->syswm.msg->msg.win.msg == WM_EXITSIZEMOVE)
        {
            limiter.queueReset();
            counter.queueReset();
            return 0;
        }
        break;
    #endif
    }
    return 1;
}

void init(int argc, char* argv[])
{
    using namespace shell;

    Options options("eggvance");
    options.add({ "rom", "ROM file" }, Options::value<fs::path>()->positional()->optional());

    OptionsResult result;
    try
    {
        result = options.parse(argc, argv);
    }
    catch (const ParseError& error)
    {
        throw shell::Error("Cannot parse command line: {}", error.what());
    }

    config.init(fs::absolute("eggvance.ini"));

    audio_ctx.init();
    input_ctx.init();
    video_ctx.init();

    Bios::init(config.bios_file);
    Color::init(config.lcd_color);

    #if SHELL_OS_WINDOWS
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    #endif
    SDL_SetEventFilter(eventFilter, NULL);

    state = UiState::Menu;

    if (const auto rom = result.find<fs::path>("rom"))
        loadRomFile(*rom);

    limiter.setFps(kRefreshRate);
}

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);

        while (state != UiState::Quit)
        {
            limiter.run([]()
            {
                doEvents();

                switch (state)
                {
                case UiState::Pause:
                case UiState::Emulate:
                    emulate();
                    break;

                case UiState::Menu:
                    menu();
                    break;
                }
            });

            switch (state)
            {
            case UiState::Pause:
                counter.reset();
                break;

            case UiState::Menu:
                updateTitle();
                break;

            case UiState::Emulate:
                if (const auto fps = (++counter).fps())
                    updateTitle(*fps);
                break;
            }
        }

        audio_ctx.pause();
    }
    catch (const std::exception& ex)
    {
        showMessageBox("Exception", ex.what());
    }
    return 0;
}
