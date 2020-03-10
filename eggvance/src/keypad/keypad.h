#pragma once

#include "io/keypadio.h"

class Keypad
{
public:
    friend class IO;

    void process();

private:
    KeypadIO io;
};

extern Keypad keypad;
