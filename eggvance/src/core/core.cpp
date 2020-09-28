#include "core.h"

#include <eggcpt/filesystem.h>
#include <eggcpt/fmt.h>
#include <eggcpt/options.h>
#include <eggcpt/utility.h>

#include "arm/arm.h"
#include "base/config.h"
#include "base/logging.h"
#include "core/audiocontext.h"
#include "core/inputcontext.h"
#include "core/videocontext.h"
#include "dma/dma.h"
#include "gamepad/gamepad.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timer.h"

void core::init(int argc, char* argv[])
{
    using namespace eggcpt;

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

        auto rom = result.find<fs::path>("rom");
        auto sav = result.find<fs::path>("--save");
        auto cfg = result.find<fs::path>("--config");

        config.load(fs::makeAbsolute(*cfg));

        mmu.bios.init(config.bios_file);

        audio_ctx.init();
        input_ctx.init();
        video_ctx.init();

        if (rom.has_value())
            mmu.gamepak.load(*rom);

        if (sav.has_value())
            mmu.gamepak.loadSave(*sav);
    }
    catch (const eggcpt::ParseError& error)
    {
        fmt::print(options.help());

        EGGCPT_LOG_FATAL("Cannot parse command line because of error '{}'", error.what());
    }
}

void core::reset()
{
    mmu.reset();
    ppu.reset();

    eggcpt::reconstruct(arm);
    eggcpt::reconstruct(dma);
    eggcpt::reconstruct(gamepad);
    eggcpt::reconstruct(timer);
}

void core::frame()
{
    gamepad.poll();

    uint visible = 160;
    while (visible--)
    {
        arm.run(960);
        ppu.scanline();
        ppu.hblank();
        arm.run(272);
        ppu.next();
    }

    ppu.vblank();

    uint invisible = 68;
    while (invisible--)
    {
        arm.run(1232);
        ppu.next();
    }

    ppu.io.dispstat.hblank = false;
    ppu.io.dispstat.vblank = false;
    ppu.present();
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
