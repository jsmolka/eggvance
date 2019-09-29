#include "keycontrol.h"

void KeyControl::reset()
{
    keys      = 0;
    irq       = 0;
    irq_logic = 0;
}
