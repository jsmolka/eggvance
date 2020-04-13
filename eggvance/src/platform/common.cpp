#include "common.h"

#include <fmt/format.h>

#include "arm/arm.h"
#include "dma/dmac.h"
#include "keypad/keypad.h"
#include "interrupt/irqhandler.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timerc.h"

void common::init(
    int argc, char* argv[], 
    const std::shared_ptr<AudioDevice>& audio_device, 
    const std::shared_ptr<InputDevice>& input_device, 
    const std::shared_ptr<VideoDevice>& video_device)
{
    fs::init(argv[0]);
    config.init("eggvance.toml");
    mmu.bios.init(config.bios_file);

    audio_device->init();
    input_device->init();
    video_device->init();

    ::audio_device = audio_device;
    ::input_device = input_device;
    ::video_device = video_device;

    switch (argc)
    {
    case 2: mmu.gamepak.load(argv[1]); break;
    case 3: mmu.gamepak.load(argv[1], argv[2]); break;
    }
}

void common::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();
    dmac = DMAController();
    irqh = IRQHandler();
    keypad = Keypad();
    timerc.reset();
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

    video_device->setWindowTitle(title);
}

void common::updateWindowTitle(double fps)
{
    const auto title = fmt::format(
        mmu.gamepak.header.title.empty()
            ? "eggvance - {1:.1f} fps"
            : "eggvance - {0} - {1:.1f} fps",
        mmu.gamepak.header.title, fps
    );

    video_device->setWindowTitle(title);
}
