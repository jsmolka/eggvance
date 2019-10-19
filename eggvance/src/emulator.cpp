#include "emulator.h"

#include <filesystem>

// Todo: remove once finished
#include <SDL2/SDL.h>
#include <fmt/printf.h>

#include "arm/arm.h"
#include "mmu/mmu.h"

namespace fs = std::filesystem;

void Emulator::reset()
{
    arm.reset();
}

bool Emulator::init(int argc, char* argv[])
{
    // Don't use cwd
    //if (!mmu.bios.init())
        // Show error
        //return false;

    if (argc > 1)
    {
        std::string file(argv[1]);
        if (fs::is_regular_file(file))
            mmu.gamepak.load(file);
    }
    return true;
}

void Emulator::run()
{
    if (mmu.gamepak.size() == 0)
    {
        // Show drop screen
    }
    reset();

    arm.run(5000);

    fmt::printf("r7: %d, r12: %d\n", arm.regs[7], arm.regs[12]);

    while (true);
}

void Emulator::frame()
{
    for (int line = 0; line < 160; ++line)
    {
        //arm.emulate(960);
        //ppu.scanline();
        //ppu.hblank();
        //arm.emulate(272);
        //ppu.next();
    }

    //ppu.vblank();
    for (int line = 0; line < 68; ++line)
    {
        //arm.emulate(960 + 272);
        //ppu.next();
    }
    //ppu.present();
}
