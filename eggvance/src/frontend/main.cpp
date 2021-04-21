#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_internal.h>
#include <shell/options.h>
#include <shell/utility.h>

#include "audiocontext.h"
#include "framecounter.h"
#include "frameratelimiter.h"
#include "inputcontext.h"
#include "nfd.h"
#include "opengl.h"
#include "videocontext.h"
#include "apu/apu.h"
#include "arm/arm.h"
#include "base/config.h"
#include "dma/dma.h"
#include "gamepak/gamepak.h"
#include "keypad/keypad.h"
#include "ppu/color.h"
#include "ppu/ppu.h"
#include "scheduler/scheduler.h"
#include "sio/sio.h"
#include "timer/timer.h"

enum class State { Quit, Menu, Run, Pause };

State state;
FrameCounter counter;
FrameRateLimiter limiter;
SDL_Scancode* select_scancode = nullptr;
SDL_GameControllerButton* select_button = nullptr;
bool active = false;

void updateUiVisible()
{
    using Clock = std::chrono::high_resolution_clock;
    using Time  = std::chrono::high_resolution_clock::time_point;

    int dx = 0;
    int dy = 0;
    SDL_GetRelativeMouseState(&dx, &dy);

    static Time moved;

    if (dx != 0 || dy != 0 || active || state == State::Menu)
        moved = Clock::now();

    SDL_ShowCursor((Clock::now() - moved) < std::chrono::milliseconds(2500));
}

bool isUiVisible()
{
    return SDL_ShowCursor(-1) == SDL_ENABLE;
}

bool isRunning()
{
    return state == State::Run || state == State::Pause;
}

void updateTitle()
{
    const auto title = shell::format(
        gamepak.rom.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        gamepak.rom.title);

    video_ctx.setTitle(title);
}

void updateTitle(double fps)
{
    const auto title = shell::format(
        gamepak.rom.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        gamepak.rom.title, fps);

    video_ctx.setTitle(title);
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

    if (gamepak.rom.empty())
        state = State::Menu;
    else
        state = State::Run;
}

void queueReset()
{
    limiter.queueReset();
    counter.queueReset();
}

void load(const std::optional<fs::path>& rom, const std::optional<fs::path>& sav)
{
    if (rom || (sav && gamepak.rom.size()))
    {
        audio_ctx.pause();

        if (rom)
            config.recent.push(*rom);

        gamepak.load(
            rom.value_or(fs::path()),
            sav.value_or(fs::path()));

        reset();

        audio_ctx.unpause();
    }
    queueReset();
}

void setFastForward(double fast_forward)
{
    if (limiter.isFastForward())
        limiter.setFastForward(fast_forward);

    config.fast_forward = fast_forward;
}

bool doSelectKey(const SDL_KeyboardEvent& event)
{
    if (event.keysym.scancode == SDL_SCANCODE_ESCAPE)
    {
        select_button = nullptr;
        select_scancode = nullptr;
    }

    if (!select_scancode)
        return false;

    *select_scancode = event.keysym.scancode;
     select_scancode = nullptr;

    return true;
}

bool doSelectButton(const SDL_ControllerButtonEvent& event)
{
    if (!select_button)
        return false;

    *select_button = SDL_GameControllerButton(event.button);
     select_button = nullptr;

    return true;
}

bool doShortcuts(const SDL_KeyboardEvent& event)
{
    if ((event.keysym.mod & KMOD_CTRL) == 0)
        return false;

    switch (event.keysym.scancode)
    {
    case SDL_SCANCODE_O:
        load(openFileDialog("gba"), std::nullopt);
        return true;

    case SDL_SCANCODE_R:
        if (isRunning())
            reset();
        return true;

    case SDL_SCANCODE_P:
        switch (state)
        {
        case State::Run:
            state = State::Pause;
            break;

        case State::Pause:
            state = State::Run;
            break;
        }
        return true;

    case SDL_SCANCODE_LSHIFT:
    case SDL_SCANCODE_RSHIFT:
        limiter.setFastForward(limiter.isFastForward() ? 1 : config.fast_forward);
        return true;

    case SDL_SCANCODE_1:
        setFastForward(1'000'000);
        return true;

    case SDL_SCANCODE_2:
        setFastForward(2);
        return true;

    case SDL_SCANCODE_3:
        setFastForward(3);
        return true;

    case SDL_SCANCODE_4:
        setFastForward(4);
        return true;

    case SDL_SCANCODE_5:
        setFastForward(5);
        return true;

    case SDL_SCANCODE_6:
        setFastForward(6);
        return true;

    case SDL_SCANCODE_7:
        setFastForward(7);
        return true;

    case SDL_SCANCODE_8:
        setFastForward(8);
        return true;

    case SDL_SCANCODE_F:
        SDL_SetWindowFullscreen(video_ctx.window, SDL_GetWindowFlags(video_ctx.window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
        return true;

    case SDL_SCANCODE_M:
        config.mute ^= true;
        return true;
    }
    return false;
}

void doEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            state = State::Quit;
            return;

        case SDL_KEYDOWN:
            if (doSelectKey(event.key) || doShortcuts(event.key))
                break;
            [[fallthrough]];

        case SDL_KEYUP:
            input_ctx.update();
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            if (doSelectButton(event.cbutton))
                break;
            [[fallthrough]];

        case SDL_CONTROLLERBUTTONUP:
            input_ctx.update();
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.deviceEvent(event.cdevice);
            break;
        }
    }
    updateUiVisible();
}

namespace ImGui
{

bool BrowseButton(const fs::path& path)
{
    return ImGui::Button(path.empty()
        ? "Browse..."
        : path.u8string().c_str());
}

bool BeginPopup(const char* title, bool& open, bool can_close)
{
    constexpr auto kWindowFlags =
          ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_AlwaysAutoResize;

    if (open)
    {
        SetNextWindowPos(GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        OpenPopup(title);
        open = BeginPopupModal(title, can_close ? &open : nullptr, kWindowFlags);
    }
    return open;
}

bool BeginSettingsWindow(const char* title, bool& open)
{
    return BeginPopup(title, open, true)
        && BeginTable(title, 2, ImGuiTableFlags_SizingFixedFit);
}

void EndSettingsWindow()
{
    if (ImGui::GetCurrentContext()->CurrentTable)
        ImGui::EndTable();

    ImGui::EndPopup();
}

void SettingsLabel(const char* text)
{
    ImGui::TableNextColumn();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
    ImGui::Text(text);
    ImGui::TableNextColumn();
}

}  // namespace ImGui

float doUi()
{
           bool show_menu       = false;
    static bool show_settings   = false;
    static bool show_keyboard   = false;
    static bool show_controller = false;

    if (!isUiVisible())
        return 0;

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(video_ctx.window);

    ImGui::NewFrame();
    
    float height = 0.0f;
    if (ImGui::BeginMainMenuBar())
    {
        height = ImGui::GetWindowHeight() / 2.0f;

        if (ImGui::BeginMenu("File"))
        {
            show_menu = true;

            if (ImGui::MenuItem("Open ROM", "Ctrl+O"))
                load(openFileDialog("gba"), std::nullopt);

            if (ImGui::MenuItem("Open save", nullptr, false, isRunning()))
                load(std::nullopt, openFileDialog("sav"));

            if (ImGui::BeginMenu("Recent", !config.recent.isEmpty()))
            {
                for (const auto& file : config.recent)
                {
                    if (file.empty())
                        break;

                    if (ImGui::MenuItem(file.u8string().c_str()))
                        load(file, std::nullopt);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Preferences"))
            {
                if (ImGui::MenuItem("Settings"))
                    show_settings = true;

                if (ImGui::MenuItem("Keyboard config"))
                    show_keyboard = true;

                if (ImGui::MenuItem("Controller config"))
                    show_controller = true;

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
                state = State::Quit;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Emulation"))
        {
            show_menu = true;

            if (ImGui::MenuItem("Reset", "Ctrl+R", nullptr, isRunning()))
                reset();

            if (ImGui::MenuItem("Pause", "Ctrl+P", state == State::Pause, isRunning()))
            {
                switch (state)
                {
                case State::Run:
                    state = State::Pause;
                    break;

                case State::Pause:
                    state = State::Run;
                    break;
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Fast forward", "Ctrl+Shift", limiter.isFastForward()))
                limiter.setFastForward(limiter.isFastForward() ? 1 : config.fast_forward);

            if (ImGui::BeginMenu("Fast forward speed"))
            {
                uint multiplier = 1'000'000;

                if (ImGui::MenuItem("Unbound", "Ctrl+1", config.fast_forward == multiplier))
                    setFastForward(multiplier);

                ImGui::Separator();

                for (multiplier = 2; multiplier <= 8; ++multiplier)
                {
                    std::string text = shell::format("{}x", multiplier);
                    std::string shortcut = shell::format("Ctrl+{}", multiplier);

                    if (ImGui::MenuItem(text.c_str(), shortcut.c_str(), config.fast_forward == multiplier))
                        setFastForward(multiplier);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Audio/Video"))
        {
            show_menu = true;

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
                    SDL_SetWindowFullscreen(video_ctx.window, fullscreen ^ SDL_WINDOW_FULLSCREEN_DESKTOP);

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

            ImGui::Separator();

            if (ImGui::BeginMenu("Video layers"))
            {
                static constexpr std::pair<std::string_view, uint> kLayers[] =
                {
                    { "Background 0", 1 << 0 },
                    { "Background 1", 1 << 1 },
                    { "Background 2", 1 << 2 },
                    { "Background 3", 1 << 3 },
                    { "Objects",      1 << 4 }
                };

                for (const auto& [text, mask] : kLayers)
                {
                    if (ImGui::MenuItem(text.data(), nullptr, config.video_layers & mask))
                        config.video_layers ^= mask;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Audio channels"))
            {
                static constexpr std::pair<std::string_view, uint> kChannels[] =
                {
                    { "Square 1", 1 << 0 },
                    { "Square 2", 1 << 1 },
                    { "Wave",     1 << 2 },
                    { "Noise",    1 << 3 },
                    { "FIFO A",   1 << 4 },
                    { "FIFO B",   1 << 5 }
                };

                for (const auto& [text, mask] : kChannels)
                {
                    if (ImGui::MenuItem(text.data(), nullptr, config.audio_channels & mask))
                        config.audio_channels ^= mask;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::BeginSettingsWindow("Settings", show_settings))
    {
        ImGui::PushID("Save");
        {
            ImGui::SettingsLabel("Save path");
            if (ImGui::BrowseButton(config.save_path))
            {
                if (const auto path = openPathDialog())
                    config.save_path = *path;

                queueReset();
            }
            if (!config.save_path.empty())
            {
                ImGui::SameLine();
                if (ImGui::Button("Clear"))
                    config.save_path = fs::path();
            }
        }
        ImGui::PopID();

        ImGui::PushID("BIOS");
        {
            ImGui::SettingsLabel("BIOS file");
            if (ImGui::BrowseButton(config.bios_file))
            {
                if (const auto file = openFileDialog())
                {
                    if (file != config.bios_file)
                    {
                        config.bios_file = *file;
                        Bios::init(config.bios_file);

                        if (isRunning())
                            reset();
                    }
                }
                queueReset();
            }
            if (!config.bios_file.empty())
            {
                ImGui::SameLine();
                if (ImGui::Button("Clear"))
                {
                    config.bios_file = fs::path();
                    Bios::init(config.bios_file);

                    if (isRunning())
                        reset();
                }
            }
        }
        ImGui::PopID();

        ImGui::SettingsLabel("Skip BIOS");
        ImGui::Checkbox("", &config.bios_skip);

        ImGui::EndSettingsWindow();
    }

    if (ImGui::BeginSettingsWindow("Keyboard config", show_keyboard))
    {
        auto map = [](const char* label, SDL_Scancode& scancode)
        {
            std::string text = SDL_GetScancodeName(scancode);
            shell::toUpper(text);

            ImGui::SettingsLabel(label);
            if (ImGui::Button(text.c_str()))
                select_scancode = &scancode;
        };

        map("A              ", config.keyboard.a);
        map("B              ", config.keyboard.b);
        map("Up             ", config.keyboard.up);
        map("Down           ", config.keyboard.down);
        map("Left           ", config.keyboard.left);
        map("Right          ", config.keyboard.right);
        map("Start          ", config.keyboard.start);
        map("Select         ", config.keyboard.select);
        map("L              ", config.keyboard.l);
        map("R              ", config.keyboard.r);
        
        bool show = select_scancode;
        if (ImGui::BeginPopup("Press button", show, false))
        {
            ImGui::Text("Press button or escape");
            ImGui::EndPopup();
        }
        ImGui::EndSettingsWindow();
    }

    if (ImGui::BeginSettingsWindow("Controller config", show_controller))
    {
        auto map = [](const char* label, SDL_GameControllerButton& button)
        {
            std::string text = "NONE";
            if (SDL_GameControllerGetStringForButton(button))
            {
                text = SDL_GameControllerGetStringForButton(button);
                shell::toUpper(text);
            }

            ImGui::SettingsLabel(label);
            if (ImGui::Button(text.c_str()))
                select_button = &button;
        };

        map("A              ", config.controller.a);
        map("B              ", config.controller.b);
        map("Up             ", config.controller.up);
        map("Down           ", config.controller.down);
        map("Left           ", config.controller.left);
        map("Right          ", config.controller.right);
        map("Start          ", config.controller.start);
        map("Select         ", config.controller.select);
        map("L              ", config.controller.l);
        map("R              ", config.controller.r);
        
        bool show = select_button;
        if (ImGui::BeginPopup("Press button", show, false))
        {
            ImGui::Text("Press button or escape");
            ImGui::EndPopup();
        }
        ImGui::EndSettingsWindow();
    }

    active = ImGui::IsAnyItemHovered()
        || show_menu
        || show_settings
        || show_keyboard
        || show_controller;

    ImGui::Render();

    return height;
}

void renderUi()
{
    if (isUiVisible())
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void frame(State state)
{
    if (state == State::Run)
    {
        constexpr auto kPixelsHor   = kScreenW + 68;
        constexpr auto kPixelsVer   = kScreenH + 68;
        constexpr auto kPixelCycles = 4;
        constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

        keypad.update();
        arm.run(kFrameCycles);
    }
    else
    {
        video_ctx.renderFrame();
    }

    doUi();
    renderUi();
    video_ctx.swapWindow();
}

void menu()
{
    float height = doUi();
    video_ctx.renderIcon(height);
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
            doEvents();

            video_ctx.updateViewport();

            switch (state)
            {
            case State::Pause:
            case State::Run:
                frame(State::Pause);
                break;

            case State::Menu:
                menu();
                break;
            }
            return 0;
        }
        break;
    
    #if SHELL_OS_WINDOWS
    case SDL_SYSWMEVENT:
        if (event->syswm.msg->msg.win.msg == WM_EXITSIZEMOVE)
        {
            queueReset();
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
    options.add({ "rom",       "ROM file"          }, Options::value<fs::path>()->positional()->optional());
    options.add({ "-s,--save", "Save file", "file" }, Options::value<fs::path>()->optional());

    OptionsResult result;
    try
    {
        result = options.parse(argc, argv);
    }
    catch (const ParseError& error)
    {
        shell::print("Cannot parse command line: {}\n", error.what());

        std::exit(1);
    }

    config.init();

    audio_ctx.init();
    input_ctx.init();
    video_ctx.init();

    Bios::init(config.bios_file);
    Color::init(config.color_correct);

    #if SHELL_OS_WINDOWS
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    #endif
    SDL_SetEventFilter(eventFilter, NULL);

    state = State::Menu;

    const auto rom = result.find<fs::path>("rom");
    const auto sav = result.find<fs::path>("--save");

    load(rom, sav);
}

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);

        while (state != State::Quit)
        {
            limiter.run([]()
            {
                doEvents();

                switch (state)
                {
                case State::Pause:
                case State::Run:
                    frame(state);
                    break;

                case State::Menu:
                    menu();
                    break;
                }
            });

            switch (state)
            {
            case State::Pause:
                counter.reset();
                break;

            case State::Menu:
                updateTitle();
                break;

            case State::Run:
                if (const auto fps = (++counter).fps())
                    updateTitle(*fps);
                break;
            }
        }

        audio_ctx.pause();
        
        return 0;
    }
    catch (const std::exception& ex)
    {
        video_ctx.showMessageBox("Exception", ex.what());

        return 1;
    }
}
