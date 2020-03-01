#pragma once

#include "registers/serial.h"

class Serial
{
public:
    friend class IO;

private:
    SerialIO io;
};

extern Serial serial;
