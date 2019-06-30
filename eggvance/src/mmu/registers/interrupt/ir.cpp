#include "ir.h"

Ir::Ir(u16& data)
    : Register<u16>(data)
    , vblank(data)
    , hblank(data)
    , vcount_match(data)
    , timer0_overflow(data)
    , timer1_overflow(data)
    , timer2_overflow(data)
    , timer3_overflow(data)
    , serial(data)
    , dma0(data)
    , dma1(data)
    , dma2(data)
    , dma3(data)
    , keypad(data)
    , gamepak(data)
{

}
