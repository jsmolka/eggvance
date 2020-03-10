#include "platform.h"

#include <fmt/format.h>

#include "audiodevice.h"
#include "inputdevice.h"
#include "framecounter.h"
#include "arm/arm.h"
#include "dma/dmac.h"
#include "interrupt/irqhandler.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timerc.h"

void Platform::init(int argc, char* argv[])
{
    initHookBeg();

    fs::init(argv[0]);
    config.init("eggvance.toml");
    mmu.bios.init(config.bios_file);

    audio_device->init();
    input_device->init();
    video_device->init();

    if (argc > 1)
        mmu.gamepak.load(argv[1]);

    initHookEnd();
}

void Platform::main()
{
    mainHookBeg();

    reset();
    updateTitle();

    FrameCounter counter;

    while (running)
    {
        synchronizer.beginFrame();

        processEvents();
        keypad.process();
        emulateFrame();

        synchronizer.endFrame();

        double value = 0;
        if ((++counter).queryFps(value))
            video_device->title(fmt::format("{} - {:.1f} fps", title, value));
    }

    mainHookEnd();
}

void Platform::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();
    dmac = DMAController();
    irqh = IRQHandler();
    keypad = Keypad();
    timerc.reset();
}

void Platform::updateTitle()
{
    title = mmu.gamepak.header.title.empty()
        ? "eggvance"
        : "eggvance - " + mmu.gamepak.header.title;

    video_device->title(title);
}

void Platform::emulateFrame()
{ 
    for (uint x = 0; x < 160; ++x)
    {
        arm.run(960);
        ppu.scanline();
        ppu.hblank();
        arm.run(272);
        ppu.next();
    }

    ppu.vblank();
    for (uint x = 0; x < 68; ++x)
    {
        arm.run(1232);
        ppu.next();
    }

    ppu.io.dispstat.hblank = false;
    ppu.io.dispstat.vblank = false;

    ppu.present();
}
