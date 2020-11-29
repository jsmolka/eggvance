#include "io.h"

#include "arm.h"
#include "constants.h"

template<uint Index>
void HaltControl::write(u8 byte)
{
    RegisterW::write<Index>(byte);

    arm.state |= kStateHalt;
}

template void HaltControl::write<0>(u8);

WaitControl::WaitControl()
{
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

    for (uint x = 0; x < 2; ++x)
    {
        cycles_half[0x8 + x][kN] = kNonSeq[ws0_n];
        cycles_word[0x8 + x][kN] = kNonSeq[ws0_n] + kWs0Seq[ws0_s];
        cycles_half[0xA + x][kN] = kNonSeq[ws1_n];
        cycles_word[0XA + x][kN] = kNonSeq[ws1_n] + kWs1Seq[ws1_s];
        cycles_half[0xC + x][kN] = kNonSeq[ws2_n];
        cycles_word[0xC + x][kN] = kNonSeq[ws2_n] + kWs2Seq[ws2_s];
        cycles_half[0xE + x][kN] = kNonSeq[sram];
        cycles_word[0xE + x][kN] = kNonSeq[sram];
        
        cycles_half[0x8 + x][kS] = kWs0Seq[ws0_s];
        cycles_word[0x8 + x][kS] = kWs0Seq[ws0_s] * 2;
        cycles_half[0xA + x][kS] = kWs1Seq[ws1_s];
        cycles_word[0XA + x][kS] = kWs1Seq[ws1_s] * 2;
        cycles_half[0xC + x][kS] = kWs2Seq[ws2_s];
        cycles_word[0xC + x][kS] = kWs2Seq[ws2_s] * 2;
        cycles_half[0xE + x][kS] = kNonSeq[sram];
        cycles_word[0xE + x][kS] = kNonSeq[sram];
    }
}
