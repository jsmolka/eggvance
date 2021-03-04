#include "io.h"

#include "arm.h"
#include "constants.h"

void HaltControl::write(uint index, u8 byte)
{
    XRegisterW::write(index, byte);

    arm.state |= kStateHalt;
}

WaitControl::WaitControl()
{
    update();
}

void WaitControl::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

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

void WaitControl::update()
{
    static constexpr int kNonSeq[4] = { 5, 4, 3, 9 };
    static constexpr int kWs0Seq[2] = { 3, 2 };
    static constexpr int kWs1Seq[2] = { 5, 2 };
    static constexpr int kWs2Seq[2] = { 9, 2 };

    constexpr uint kN = static_cast<uint>(Access::NonSequential);
    constexpr uint kS = static_cast<uint>(Access::Sequential);

    cycles_half[0][kN] = kNonSeq[ws0_n];
    cycles_word[0][kN] = kNonSeq[ws0_n] + kWs0Seq[ws0_s];
    cycles_half[1][kN] = kNonSeq[ws1_n];
    cycles_word[1][kN] = kNonSeq[ws1_n] + kWs1Seq[ws1_s];
    cycles_half[2][kN] = kNonSeq[ws2_n];
    cycles_word[2][kN] = kNonSeq[ws2_n] + kWs2Seq[ws2_s];
    cycles_half[3][kN] = kNonSeq[sram];
    cycles_word[3][kN] = kNonSeq[sram];
        
    cycles_half[0][kS] = kWs0Seq[ws0_s];
    cycles_word[0][kS] = kWs0Seq[ws0_s] * 2;
    cycles_half[1][kS] = kWs1Seq[ws1_s];
    cycles_word[1][kS] = kWs1Seq[ws1_s] * 2;
    cycles_half[2][kS] = kWs2Seq[ws2_s];
    cycles_word[2][kS] = kWs2Seq[ws2_s] * 2;
    cycles_half[3][kS] = kNonSeq[sram];
    cycles_word[3][kS] = kNonSeq[sram];
}

IrqMaster::operator bool() const
{
    return value & mask;
}

void IrqMaster::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

    on_write();

}

IrqEnable::operator u16() const
{
    return value & mask;
}

void IrqEnable::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

    on_write();
}

IrqRequest::operator u16() const
{
    return value & mask;
}

void IrqRequest::write(uint index, u8 byte)
{
    bytes[index] &= ~(byte & bit::byte(mask, index));

    on_write();
}
