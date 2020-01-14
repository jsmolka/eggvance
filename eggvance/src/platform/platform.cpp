#include "platform.h"

#include <fmt/format.h>

#include "audiodevice.h"
#include "inputdevice.h"
#include "videodevice.h"
#include "framecounter.h"
#include "arm/arm.h"
#include "common/config.h"
#include "common/fs.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "system/dmacontroller.h"
#include "system/keypad.h"
#include "system/timercontroller.h"

void Platform::init(int argc, char* argv[])
{
    initHookBeg();

    fs::init(argv[0]);

    config.init("eggvance.toml");
    mmu.bios.init(config.bios_file);

    audio_device->init();
    input_device->init();
    video_device->init();

    if (argc > 1) mmu.gamepak.load(argv[1]);

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

        double value = 0;
        if ((++counter).fps(value))
            video_device->title(fmt::format("{} - {:.1f} fps", title, value));

        synchronizer.endFrame();
    }

    mainHookEnd();
}

void Platform::reset()
{
    arm.reset();
    mmu.reset();
    ppu.reset();
    dmac.reset();
    keypad.reset();
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
    for (int x = 0; x < 160; ++x)
    {
        arm.run(960);
        ppu.scanline();
        ppu.hblank();
        arm.run(272);
        ppu.next();
    }

    ppu.vblank();
    for (int x = 0; x < 68; ++x)
    {
        arm.run(1232);
        ppu.next();
    }

    ppu.io.dispstat.hblank = false;
    ppu.io.dispstat.vblank = false;

    ppu.present();
}
