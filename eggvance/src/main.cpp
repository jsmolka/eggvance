#include "emulator.h"

int main(int argc, char* argv[])
{
    Emulator emulator;
    if (emulator.init(argc, argv) != 0)
        return 1;

    #undef main
    return emulator.main();
}
