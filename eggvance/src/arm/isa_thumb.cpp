#include "arm.h"

void ARM::Thumb_SoftwareInterrupt(u16 instr)
{
    SWI();
}

void ARM::Thumb_UnconditionalBranch(u16 instr)
{
    int offset = bits<0, 11>(instr);

    cycle<Access::Nonseq>(pc + 4);

    offset = signExtend<11>(offset);
    offset <<= 1;

    pc += offset;
    refill<State::Thumb>();
}

void ARM::Thumb_Undefined(u16 instr)
{

}
