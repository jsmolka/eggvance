#include "io.h"

#include "arm.h"
#include "base/config.h"

PostFlag::PostFlag()
{
    if (config.bios_skip)
    {
        write(0, 0x01);
    }
}

void HaltControl::write(uint index, u8 byte)
{
    RegisterW::write(index, byte);

    arm.state |= State::Halt;
}

WaitControl::WaitControl()
{
    update();
}

void WaitControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    if (index == 0)
    {
        sram  = bit::seq<0, 2>(byte);
        ws0_n = bit::seq<2, 2>(byte);
        ws0_s = bit::seq<4, 1>(byte);
        ws1_n = bit::seq<5, 2>(byte);
        ws1_s = bit::seq<7, 1>(byte);
    }
    else
    {
        ws2_n    = bit::seq<0, 2>(byte);
        ws2_s    = bit::seq<2, 1>(byte);
        prefetch = bit::seq<6, 1>(byte);
    }
    update();
}

u64 WaitControl::waitHalf(u32 addr, Access access) const
{
    return wait.half[(addr >> 25) & 0x3][uint(access)];
}

u64 WaitControl::waitWord(u32 addr, Access access) const
{
    return wait.word[(addr >> 25) & 0x3][uint(access)];
}

void WaitControl::update()
{
    static constexpr u64 kNonSeq[4] = { 5, 4, 3, 9 };
    static constexpr u64 kWs0Seq[2] = { 3, 2 };
    static constexpr u64 kWs1Seq[2] = { 5, 2 };
    static constexpr u64 kWs2Seq[2] = { 9, 2 };

    constexpr uint kN = uint(Access::NonSequential);
    constexpr uint kS = uint(Access::Sequential);

    wait.half[0][kN] = kNonSeq[ws0_n];
    wait.word[0][kN] = kNonSeq[ws0_n] + kWs0Seq[ws0_s];
    wait.half[1][kN] = kNonSeq[ws1_n];
    wait.word[1][kN] = kNonSeq[ws1_n] + kWs1Seq[ws1_s];
    wait.half[2][kN] = kNonSeq[ws2_n];
    wait.word[2][kN] = kNonSeq[ws2_n] + kWs2Seq[ws2_s];
    wait.half[3][kN] = kNonSeq[sram];
    wait.word[3][kN] = kNonSeq[sram];
        
    wait.half[0][kS] = kWs0Seq[ws0_s];
    wait.word[0][kS] = kWs0Seq[ws0_s] * 2;
    wait.half[1][kS] = kWs1Seq[ws1_s];
    wait.word[1][kS] = kWs1Seq[ws1_s] * 2;
    wait.half[2][kS] = kWs2Seq[ws2_s];
    wait.word[2][kS] = kWs2Seq[ws2_s] * 2;
    wait.half[3][kS] = kNonSeq[sram];
    wait.word[3][kS] = kNonSeq[sram];
}

IrqMaster::operator bool() const
{
    return data;
}

void IrqMaster::write(uint index, u8 byte)
{
    Register::write(index, byte);

    arm.interruptProcess();

}

IrqEnable::operator u16() const
{
    return data;
}

void IrqEnable::write(uint index, u8 byte)
{
    Register::write(index, byte);

    arm.interruptProcess();
}

IrqRequest& IrqRequest::operator|=(Irq irq)
{
    data |= irq;

    return *this;
}

IrqRequest::operator u16() const
{
    return data;
}

void IrqRequest::write(uint index, u8 byte)
{
    bit::byteRef(data, index) &= ~(byte & bit::byte(mask, index));

    arm.interruptProcess();
}
