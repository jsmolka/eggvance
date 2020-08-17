#pragma once

#include "keypad/io.h"

class Core;

class Keypad
{
public:
    Keypad(Core& core);

    void update();

    KeypadIo io;

private:
    Core& core;
};
