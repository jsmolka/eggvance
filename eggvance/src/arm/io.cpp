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
