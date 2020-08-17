#pragma once

#include <string>

namespace common
{
    void init(int argc, char* argv[]);

    void reset();
    void frame();

    void updateWindowTitle();
    void updateWindowTitle(double fps);
}
