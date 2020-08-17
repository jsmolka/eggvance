#include "common.h"

#include <fmt/format.h>

#include "arm/arm.h"
#include "base/util.h"
#include "dma/dmac.h"
#include "irq/irqh.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timerc.h"
#include "core/core.h"

void common::init(int argc, char* argv[])
{
    config.init(argc, argv);
    mmu.bios.init(config.bios_file);

    g_core.context.init();

    switch (argc)
    {
    case 2: mmu.gamepak.load(fs::u8path(argv[1])); break;
    case 3: mmu.gamepak.load(fs::u8path(argv[1]), fs::u8path(argv[2])); break;
    }
}

void common::reset()
{
    mmu.reset();
    ppu.reset();

    util::reconstruct(&arm);
    util::reconstruct(&dmac);
    util::reconstruct(&irqh);
    util::reconstruct(&keypad);
    util::reconstruct(&timerc);
}

void common::frame()
{
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

void common::updateWindowTitle()
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        mmu.gamepak.header.title
    );

    g_core.context.video.setWindowTitle(title);
}

void common::updateWindowTitle(double fps)
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        mmu.gamepak.header.title, fps
    );

    g_core.context.video.setWindowTitle(title);
}
