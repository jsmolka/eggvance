#include "core.h"

#include "base/eggcpt.h"

Core::Core()
    : arm(*this)
    , mmu(*this)
    , ppu(*this)
    , keypad(*this)
    , irqh(*this)
    , dmac(*this)
    , timerc(*this)
{

}

void Core::init(int argc, char* argv[])
{
    config.init(argc, argv);
    mmu.bios.init(config.bios_file);

    context.init();

    switch (argc)
    {
    case 2: mmu.gamepak.load(fs::u8path(argv[1])); break;
    case 3: mmu.gamepak.load(fs::u8path(argv[1]), fs::u8path(argv[2])); break;
    }
}

void Core::reset()
{
    mmu.reset();
    ppu.reset();

    eggcpt::reconstruct(arm, *this);
    eggcpt::reconstruct(dmac, *this);
    eggcpt::reconstruct(irqh, *this);
    eggcpt::reconstruct(keypad, *this);
    eggcpt::reconstruct(timerc, *this);
}

void Core::frame()
{
    keypad.update();

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

void Core::updateTitle()
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance"
            : "eggvance - {0}",
        mmu.gamepak.header.title);

    context.video.title(title);
}

void Core::updateTitle(double fps)
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        mmu.gamepak.header.title, fps);

    context.video.title(title);
}
