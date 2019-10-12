#include "emulator.h"

// Todo: remove once finished
#include <SDL2/SDL.h>

#include <filesystem>

namespace fs = std::filesystem;

#undef main

Emulator::Emulator()
{
    reset();

    arm.mem = &mem;
}

void Emulator::reset()
{
    arm.reset();
}

bool Emulator::init(int argc, char* argv[])
{
    if (argc > 1)
    {
        if (fs::exists(argv[1]) && fs::is_regular_file(argv[1]))
        {
            mem.cartridge.load(argv[1]);
        }
    }
    return true;
}

#include <fmt/printf.h>
void Emulator::main()
{
    if (mem.cartridge.size() == 0)
    {
        // Show drop screen
    }

    int t = 1500;
    while (t--)
    {
        arm.emulate();
    }

    fmt::printf("r7: %d, r12: %d\n", arm.regs[7], arm.regs[12]);

    while (true);
}

// Todo: leftovers are possible
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
