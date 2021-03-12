#include "core.h"

#include <shell/fmt.h>
#include <shell/options.h>
#include <shell/utility.h>

#include "audiocontext.h"
#include "inputcontext.h"
#include "videocontext.h"
#include "apu/apu.h"
#include "arm/arm.h"
#include "base/config.h"
#include "base/constants.h"
#include "base/panic.h"
#include "dma/dma.h"
#include "gamepad/gamepad.h"
#include "gamepak/gamepak.h"
#include "ppu/ppu.h"
#include "scheduler/scheduler.h"
#include "sio/sio.h"
#include "timer/timer.h"

using namespace shell;

void core::init(int argc, char* argv[])
{
    Options options("eggvance");
    options.add({ "-c,--config", "Config file", "file" }, Options::value<fs::path>("eggvance.ini"));
    options.add({ "-s,--save",   "Save file",   "file" }, Options::value<fs::path>()->optional());
    options.add({       "rom",   "ROM file"            }, Options::value<fs::path>()->positional()->optional());

    try
    {
        OptionsResult result = options.parse(argc, argv);

        const auto cfg = result.find<fs::path>("--config");
        const auto sav = result.find<fs::path>("--save");
        const auto gba = result.find<fs::path>("rom");

        config.init(fs::absolute(*cfg));

        Bios::init(config.bios_file);

        audio_ctx.init();
        input_ctx.init();
        video_ctx.init();

        gamepak.load(
            gba.value_or(fs::path()),
            sav.value_or(fs::path()));
    }
    catch (const shell::ParseError& error)
    {
        fmt::print(options.help());
        panic("Cannot parse command line\n{}", error.what());
    }
}

void core::reset()
{
    gamepak.gpio->reset();
    gamepak.save->reset();

    shell::reconstruct(apu);
    shell::reconstruct(arm);
    shell::reconstruct(dma);
    shell::reconstruct(ppu);
    shell::reconstruct(gamepad);
    shell::reconstruct(sio);
    shell::reconstruct(scheduler);
    shell::reconstruct(timer);

    arm.init();
    apu.init();
    ppu.init();
}

void core::frame()
{
    constexpr auto kPixelsHor   = 240 + 68;
    constexpr auto kPixelsVer   = 160 + 68;
    constexpr auto kPixelCycles = 4;
    constexpr auto kFrameCycles = kPixelCycles * kPixelsHor * kPixelsVer;

    gamepad.poll();

    arm.run(kFrameCycles);

    ppu.present();
}

void core::updateTitle()
{
    const auto title = fmt::format(
        gamepak.rom.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        gamepak.rom.title);

    video_ctx.title(title);
}

void core::updateTitle(double fps)
{
    const auto title = fmt::format(
        gamepak.rom.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        gamepak.rom.title, fps);

    video_ctx.title(title);
}
