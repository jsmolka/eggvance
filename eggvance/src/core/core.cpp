#include "core.h"

#include <shell/fmt/format.h>
#include <shell/options.h>
#include <shell/utility.h>

#include "audiocontext.h"
#include "inputcontext.h"
#include "videocontext.h"
#include "arm/arm.h"
#include "base/config.h"
#include "base/panic.h"
#include "dma/dma.h"
#include "gamepad/gamepad.h"
#include "gamepak/gamepak.h"
#include "gpu/gpu.h"
#include "mmu/mmu.h"
#include "timer/timer.h"

void core::init(int argc, char* argv[])
{
    using namespace shell;

    Options options("eggvance");
    options.add({   "--help", "-h"         }, "Show help"         , Options::value<bool>());
    options.add({   "--save", "-s", "file" }, "Path to the save"  , Options::value<fs::path>()->optional());
    options.add({      "rom"               }, "Path to the ROM"   , Options::value<fs::path>()->positional()->optional());

    try
    {
        OptionsResult result = options.parse(argc, argv);

        if (*result.find<bool>("--help"))
        {
            fmt::print(options.help());
            std::exit(0);
        }

        const auto rom = result.find<fs::path>("rom");
        const auto sav = result.find<fs::path>("--save");

        config.init(fs::makeAbsolute("eggvance.ini"));

        Bios::init(config.bios_file);

        audio_ctx.init();
        input_ctx.init();
        video_ctx.init();

        if (rom.has_value())
        {
            gamepak.loadRom(*rom, !sav);

            if (sav.has_value())
                gamepak.loadSave(*sav);
        }
    }
    catch (const ParseError& error)
    {
        fmt::print(options.help());

        panic("Cannot parse command line arguments\nError: {}", error.what());
    }
}

void core::reset()
{
    gamepak.gpio->reset();
    gamepak.save->reset();

    shell::reconstruct(arm);
    shell::reconstruct(dma);
    shell::reconstruct(gpu);
    shell::reconstruct(gamepad);
    shell::reconstruct(mmu);
    shell::reconstruct(timer);

    arm.init();
}

void core::frame()
{
    gamepad.poll();

    uint visible = 160;
    while (visible--)
    {
        arm.run(960);
        gpu.scanline();
        gpu.hblank();
        arm.run(272);
        gpu.next();
    }

    gpu.vblank();

    uint invisible = 68;
    while (invisible--)
    {
        arm.run(1232);
        gpu.next();
    }

    gpu.present();
}

void core::updateTitle()
{
    const auto title = fmt::format(
        gamepak.header.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        gamepak.header.title);

    video_ctx.title(title);
}

void core::updateTitle(double fps)
{
    const auto title = fmt::format(
        gamepak.header.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        gamepak.header.title, fps);

    video_ctx.title(title);
}
