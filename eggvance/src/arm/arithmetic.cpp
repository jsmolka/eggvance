#include "arm.h"

u32 Arm::add(u32 op1, u32 op2, bool flags)
{
    u32 res = op1 + op2;

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCAdd(op1, op2);
        cpsr.setVAdd(op1, op2, res);
    }
    return res;
}

u32 Arm::sub(u32 op1, u32 op2, bool flags)
{
    u32 res = op1 - op2;

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCSub(op1, op2);
        cpsr.setVSub(op1, op2, res);
    }
    return res;
}

u32 Arm::adc(u32 op1, u32 op2, bool flags)
{
    u64 opc = static_cast<u64>(op2) + cpsr.c;
    u32 res = static_cast<u32>(op1 + opc);

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCAdd(op1, opc);
        cpsr.setVAdd(op1, op2, res);
    }
    return res;
}

u32 Arm::sbc(u32 op1, u32 op2, bool flags)
{
    u64 opc = static_cast<u64>(op2) - cpsr.c + 1;
    u32 res = static_cast<u32>(op1 - opc);

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCSub(op1, opc);
        cpsr.setVSub(op1, op2, res);
    }
    return res;
}
