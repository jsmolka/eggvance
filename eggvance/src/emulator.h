#pragma once

#include "args.h"

class Emulator
{
public:
    void reset();

    bool init(const Args& args);
    
    void run();

private:
    void frame();
};
