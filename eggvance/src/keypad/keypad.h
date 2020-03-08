#pragma once

#include "io/keypad_io.h"

class Keypad
{
public:
    friend class IO;

    void process();

private:
    KeypadIO io;
};

extern Keypad keypad;
