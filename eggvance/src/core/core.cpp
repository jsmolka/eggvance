#include "core.h"

#include <shell/fmt/format.h>
#include <shell/options.h>
#include <shell/utility.h>

#include "arm/arm.h"
#include "base/config.h"
#include "base/utility.h"
#include "core/audiocontext.h"
#include "core/inputcontext.h"
#include "core/videocontext.h"
#include "dma/dma.h"
#include "gamepad/gamepad.h"
#include "gpu/gpu.h"
#include "mmu/mmu.h"
#include "timer/timer.h"

void core::init(int argc, char* argv[])
{
    using namespace shell;

    Options options("eggvance");
    options.add({   "--help", "-h" }, "Show this help text"    , Options::value<bool>()->optional());
    options.add({ "--config", "-c" }, "Path to the config file", Options::value<fs::path>("eggvance.ini"));
    options.add({   "--save", "-s" }, "Path to the save file"  , Options::value<fs::path>()->optional());
    options.add({            "rom" }, "Path to the ROM file"   , Options::value<fs::path>()->positional()->optional());

    try
    {
        OptionsResult result = options.parse(argc, argv);

        if (result.has("--help"))
        {
            fmt::print(options.help());
            std::exit(0);
        }

        const auto rom = result.find<fs::path>("rom");
        const auto sav = result.find<fs::path>("--save");
        const auto cfg = result.find<fs::path>("--config");

        config.init(fs::makeAbsolute(*cfg));

        Bios::init(config.bios_file);

        audio_ctx.init();
        input_ctx.init();
        video_ctx.init();

        if (rom.has_value())
        {
            mmu.gamepak.loadRom(*rom, !sav);

            if (sav.has_value())
                mmu.gamepak.loadSave(*sav);
        }
    }
    catch (const ParseError& error)
    {
        fmt::print(options.help());

        exit("Cannot parse command line arguments\nError: {}", error.what());
    }
}

void core::reset()
{
    mmu.reset();
    gpu.reset();

    shell::reconstruct(arm);
    shell::reconstruct(dma);
    shell::reconstruct(gamepad);
    shell::reconstruct(timer);
}

void core::frame()
{
    gamepad.poll();

    for (uint visible = 0; visible < 160; ++visible)
    {
        arm.run(960);
        gpu.scanline();
        gpu.hblank();
        arm.run(272);
        gpu.next();
    }

    gpu.vblank();

    for (uint invisible = 0; invisible < 68; ++invisible)
    {
        arm.run(1232);
        gpu.next();
    }

    gpu.present();
}

void core::updateTitle()
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        mmu.gamepak.header.title);

    video_ctx.title(title);
}

void core::updateTitle(double fps)
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        mmu.gamepak.header.title, fps);

    video_ctx.title(title);
}
