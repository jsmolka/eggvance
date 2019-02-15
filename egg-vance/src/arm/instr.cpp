#include "arm.h"

// Logical Shift Left
u32 ARM::LSL(u32 value, int offset)
{
    int carry;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value << (offset - 1)) >> 31;

        value <<= offset;
    }
    // Special case LSL #0
    else
    {
        // Todo: "the shifter carry out is the old value of the CPSR C flag"?
        carry = flagC();
    }

    updateFlagZ(value);
    updateFlagN(value);
    updateFlagC(carry);

    return value;
}

// Logical Shift Right
u32 ARM::LSR(u32 value, int offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0b1;

        value >>= offset;
    }
    // Special case LSR #32 / #0
    else
    {
        // Store the MSB in the carry
        carry = value >> 31;
        // Reset the result
        value = 0;
    }

    updateFlagZ(value);
    updateFlagN(value);
    updateFlagC(carry);

    return value;
}

// Arithmetic Shift Right
u32 ARM::ASR(u32 value, int offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0b1;

        u32 msb = value & (1 << 31);
        for (int i = 0; i < offset; ++i)
        {
            value >>= 1;
            value |= msb;
        }
    }
    // Special case LSR #32 / #0
    else
    {
        // Store the MSB in the carry
        carry = value >> 31;
        // Apply carry bit to whole result
        value = carry ? 0xFFFFFFFF : 0;
    }

    updateFlagZ(value);
    updateFlagN(value);
    updateFlagC(carry);

    return value;
}

// Rotate Right
u32 ARM::ROR(u32 value, int offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        for (int i = 0; i < offset; ++i)
        {
            carry = value & 0b1;
            value >>= 1;
            value |= (carry << 31);
        }
    }
    // Special case ROR #0
    else
    {
        // Save the first bit in the carry
        carry = value & 0b1;
        // Rotate by one
        value >>= 1;
        // Change MSB to current carry
        value |= (flagC() << 31);
    }

    updateFlagZ(value);
    updateFlagN(value);
    updateFlagC(carry);

    return value;
}
