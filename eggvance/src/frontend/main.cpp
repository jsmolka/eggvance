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

namespace ImGui
{

bool DialogButton(const fs::path& file)
{
    std::string text = file.empty()
        ? "Browse..."
        : file.u8string().c_str();

    return ImGui::Button(text.c_str());
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

enum class UiState
{
    Quit,
    Menu,
    Emulate,
    Pause
} state;

FrameCounter counter;
FrameRateLimiter limiter;
SDL_Scancode* choose_scancode = nullptr;
SDL_GameControllerButton* choose_button = nullptr;
bool ui_active = false;

void updateUiVisible()
{
    using Clock = std::chrono::high_resolution_clock;
    using Point = std::chrono::high_resolution_clock::time_point;

    int dx = 0;
    int dy = 0;
    SDL_GetRelativeMouseState(&dx, &dy);

    static Point moved;

    if (dx > 0 || dy > 0 || state == UiState::Menu || ui_active)
        moved = Clock::now();

    SDL_ShowCursor((Clock::now() - moved) < std::chrono::milliseconds(2500));
}

bool isUiVisible()
{
    return SDL_ShowCursor(-1) == SDL_ENABLE;
}

void setMultiplier(double mulitplier)
{
    config.fast_forward = mulitplier;

    if (limiter.isFastForward())
        limiter.setMultiplier(mulitplier);
}

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
    if (const auto file = openFileDialog("gba"))
        loadRomFile(*file);
    
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
    if (const auto file = openFileDialog("sav"))
        loadSavFile(*file);

    limiter.queueReset();
    counter.queueReset();
}

bool doChooseButton(const SDL_ControllerButtonEvent& event)
{
    if (!choose_button)
        return false;

    *choose_button = SDL_GameControllerButton(event.button);
     choose_button = nullptr;

    return true;
}

bool handleChooseKeyboard(const SDL_KeyboardEvent& event)
{
    if (event.keysym.scancode == SDL_SCANCODE_ESCAPE)
    {
        choose_scancode = nullptr;
        choose_button = nullptr;
    }

    if (!choose_scancode)
        return false;

    *choose_scancode = event.keysym.scancode;
     choose_scancode = nullptr;

    return true;
}

bool handleShortcuts(const SDL_KeyboardEvent& event)
{
    if ((event.keysym.mod & KMOD_CTRL) == 0)
        return false;

    switch (event.keysym.scancode)
    {
    case SDL_SCANCODE_O:
        openRomFile();
        return true;

    case SDL_SCANCODE_R:
        if (state != UiState::Menu)
            reset();
        return true;

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
        return true;

    case SDL_SCANCODE_LSHIFT:
    case SDL_SCANCODE_RSHIFT:
        if (limiter.isFastForward())
            limiter.setMultiplier(1);
        else
            limiter.setMultiplier(config.fast_forward);
        return true;

    case SDL_SCANCODE_1:
        setMultiplier(1'000'000);
        return true;

    case SDL_SCANCODE_2:
        setMultiplier(2);
        return true;

    case SDL_SCANCODE_3:
        setMultiplier(3);
        return true;

    case SDL_SCANCODE_4:
        setMultiplier(4);
        return true;

    case SDL_SCANCODE_5:
        setMultiplier(5);
        return true;

    case SDL_SCANCODE_6:
        setMultiplier(6);
        return true;

    case SDL_SCANCODE_7:
        setMultiplier(7);
        return true;

    case SDL_SCANCODE_8:
        setMultiplier(8);
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

void handleEvents()
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
            if (handleChooseKeyboard(event.key) || handleShortcuts(event.key))
                break;
            [[fallthrough]];

        case SDL_KEYUP:
            input_ctx.update();
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            if (doChooseButton(event.cbutton))
                break;
            [[fallthrough]];

        case SDL_CONTROLLERBUTTONUP:
            input_ctx.update();
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input_ctx.handleDeviceEvent(event.cdevice);
            break;
        }
    }

    updateUiVisible();
}

float runUi()
{
    static bool show_menu       = false;
    static bool show_settings   = false;
    static bool show_keyboard   = false;
    static bool show_controller = false;

    if (!isUiVisible())
        return 0;

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(video_ctx.window);

    ImGui::NewFrame();
    ImGui::BeginMainMenuBar();

    float height = ImGui::GetWindowHeight() / 2.0f;

    if (ImGui::BeginMenu("File"))
    {
        show_menu = true;

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

                if (ImGui::MenuItem(file.u8string().c_str()))
                    loadRomFile(fs::path(file));
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Preferences"))
        {
            if (ImGui::MenuItem("Settings"))
                show_settings = true;

            if (ImGui::MenuItem("Keyboard map"))
                show_keyboard = true;

            if (ImGui::MenuItem("Controller map"))
                show_controller = true;

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
            state = UiState::Quit;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Emulation"))
    {
        show_menu = true;

        if (ImGui::MenuItem("Reset", "Ctrl+R", nullptr, state != UiState::Menu))
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
                limiter.setMultiplier(1);
            else
                limiter.setMultiplier(config.fast_forward);
        }

        if (ImGui::BeginMenu("Fast forward speed"))
        {
            uint multiplier = 1'000'000;

            if (ImGui::MenuItem("Unbound", "Ctrl+1", config.fast_forward == multiplier))
                setMultiplier(multiplier);

            ImGui::Separator();

            for (multiplier = 2; multiplier <= 8; ++multiplier)
            {
                std::string text = shell::format("{}x", multiplier);
                std::string shortcut = shell::format("Ctrl+{}", multiplier);

                if (ImGui::MenuItem(text.c_str(), shortcut.c_str(), config.fast_forward == multiplier))
                    setMultiplier(multiplier);
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

    if (ImGui::BeginSettingsWindow("Settings", show_settings))
    {
        ImGui::PushID("Save");
        {
            ImGui::SettingsLabel("Save path");
            if (ImGui::DialogButton(config.save_path))
            {
                if (const auto path = openPathDialog())
                    config.save_path = *path;

                limiter.reset();
                counter.reset();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear"))
                config.save_path = fs::path();
        }
        ImGui::PopID();

        ImGui::PushID("BIOS");
        {
            ImGui::SettingsLabel("BIOS file");
            if (ImGui::DialogButton(config.bios_file))
            {
                if (const auto file = openFileDialog())
                {
                    config.bios_file = *file;
                    Bios::init(config.bios_file);
                }
                limiter.reset();
                counter.reset();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear##1"))
                config.bios_file = fs::path();
        }
        ImGui::PopID();

        ImGui::SettingsLabel("Skip BIOS");
        ImGui::Checkbox("", &config.bios_skip);

        ImGui::EndSettingsWindow();
    }

    if (ImGui::BeginSettingsWindow("Keyboard map", show_keyboard))
    {
        auto map = [](const char* label, SDL_Scancode& scancode)
        {
            std::string text = SDL_GetScancodeName(scancode);
            shell::toUpper(text);

            ImGui::SettingsLabel(label);
            if (ImGui::Button(text.c_str()))
                choose_scancode = &scancode;
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
        
        bool show = choose_scancode;
        if (ImGui::BeginPopup("Press button", show, false))
        {
            ImGui::Text("Press button or escape");
            ImGui::EndPopup();
        }
        ImGui::EndSettingsWindow();
    }

    if (ImGui::BeginSettingsWindow("Controller map", show_controller))
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
                choose_button = &button;
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
        
        bool show = choose_button;
        if (ImGui::BeginPopup("Press button", show, false))
        {
            ImGui::Text("Press button or escape");
            ImGui::EndPopup();
        }
        ImGui::EndSettingsWindow();
    }

    ui_active = ImGui::IsAnyItemHovered()
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

    state = UiState::Menu;

    if (const auto rom = result.find<fs::path>("rom"))
        loadRomFile(*rom);
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
                handleEvents();

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
        video_ctx.showMessageBox("Exception", ex.what());
    }
    return 0;
}
