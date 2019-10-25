#define ZERO(x) ((x) == 0)
#define SIGN(x) ((x) >> 31)

#define CARRY_ADD(op1, op2) (op2) > (0xFFFFFFFF - (op1))
#define CARRY_SUB(op1, op2) (op2) <= (op1)

#define OVERFLOW_ADD(op1, op2, res) SIGN(op1) == SIGN(op2) && SIGN(res) != SIGN(op1)
#define OVERFLOW_SUB(op1, op2, res) SIGN(op1) != SIGN(op2) && SIGN(res) == SIGN(op2)

u32 ARM::logical(u32 result, bool flags)
{
    if (flags)
    {
        cpsr.z = ZERO(result);
        cpsr.n = SIGN(result);
    }
    return result;
}

u32 ARM::logical(u32 result, bool carry, bool flags)
{
    if (flags)
    {
        cpsr.z = ZERO(result);
        cpsr.n = SIGN(result);
        cpsr.c = carry;
    }
    return result;
}

u32 ARM::add(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 + op2;

    if (flags)
    {
        cpsr.z = ZERO(result);
        cpsr.n = SIGN(result);
        cpsr.c = CARRY_ADD(op1, op2);
        cpsr.v = OVERFLOW_ADD(op1, op2, result);
    }
    return result;
}

u32 ARM::sub(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 - op2;

    if (flags)
    {
        cpsr.z = ZERO(result);
        cpsr.n = SIGN(result);
        cpsr.c = CARRY_SUB(op1, op2);
        cpsr.v = OVERFLOW_SUB(op1, op2, result);
    }
    return result;
}

inline u32 ARM::adc(u64 op1, u64 op2, bool flags)
{
    u64 opc = op2 + cpsr.c;

    u32 result = static_cast<u32>(op1 + opc);

    if (flags)
    {
        cpsr.z = ZERO(result);
        cpsr.n = SIGN(result);
        cpsr.c = CARRY_ADD(op1, opc);
        cpsr.v = OVERFLOW_ADD(op1, op2, result);
    }
    return result;
}

inline u32 ARM::sbc(u64 op1, u64 op2, bool flags)
{
    u64 opc = op2 - cpsr.c + 1;

    u32 result = static_cast<u32>(op1 - opc);

    if (flags)
    {
        cpsr.z = ZERO(result);
        cpsr.n = SIGN(result);
        cpsr.c = CARRY_SUB(op1, opc);
        cpsr.v = OVERFLOW_SUB(op1, op2, result);
    }
    return result;
}

#undef OVERFLOW_SUB
#undef OVERFLOW_ADD

#undef CARRY_SUB
#undef CARRY_ADD

#undef SIGN
#undef ZERO
