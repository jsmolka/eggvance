#include "windowrange.h"

void WindowRange::write(int index, u8 byte)
{
    switch (index)
    {
    case 0: max = byte; break;
    case 1: min = byte; break;
    }
}
