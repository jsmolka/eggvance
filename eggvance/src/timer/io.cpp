#include "io.h"

u8 TimerCount::read(uint index) const
{
    run_channels();

    return XRegister::read(index);
}

void TimerCount::write(uint index, u8 byte)
{
    SHELL_ASSERT(index < sizeof(initial));

    reinterpret_cast<u8*>(&initial)[index] = byte;
}

TimerControl::TimerControl()
    : XRegister(0x00C7)
{

}

void TimerControl::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

    if (index == 0)
    {
        static constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

        run_channels();

        uint was_enabled = enable;

        prescaler = kPrescalers[bit::seq<0, 3>(byte)];
        cascade   = bit::seq<2, 1>(byte);
        irq       = bit::seq<6, 1>(byte);
        enable    = bit::seq<7, 1>(byte);

        on_write(!was_enabled && enable);
    }
}
