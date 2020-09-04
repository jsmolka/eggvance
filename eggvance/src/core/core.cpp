#include "core.h"

#include <eggcpt/filesystem.h>
#include <eggcpt/fmt.h>
#include <eggcpt/utility.h>

#include "arm/arm.h"
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
    config.init(argc, argv);
    mmu.bios.init(config.bios_file);

    audio_ctx.init();
    input_ctx.init();
    video_ctx.init();

    switch (argc)
    {
    case 2: mmu.gamepak.load(fs::u8path(argv[1])); break;
    case 3: mmu.gamepak.load(fs::u8path(argv[1]), fs::u8path(argv[2])); break;
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
