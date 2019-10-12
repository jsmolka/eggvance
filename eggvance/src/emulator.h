#pragma once

#include "arm/arm.h"
#include "memory/memory.h"

#undef main

class Emulator
{
public:
    Emulator();

    void reset();

    bool init(int argc, char* argv[]);
    
    void main();

private:
    void frame();

    ARM arm;
    Memory mem;
};
