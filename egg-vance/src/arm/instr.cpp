#include "arm.h"

// Logical Shift Left
u8 ARM::LSL(u32& result, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (result << (offset - 1)) >> 31;

        result <<= offset;
    }
    // Special case LSL #0
    else
    {
        // Todo: "the shifter carry out is the old value of the CPSR C flag"?
        carry = flagC();
    }
    return carry;
}

// Logical Shift Right
u8 ARM::LSR(u32& result, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (result >> (offset - 1)) & 0b1;

        result >>= offset;
    }
    // Special case LSR #32 / #0
    else
    {
        // Store the MSB in the carry
        carry = result >> 31;
        // Reset the result
        result = 0;
    }
    return carry;
}

// Arithmetic Shift Right
u8 ARM::ASR(u32& result, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (result >> (offset - 1)) & 0b1;

        u32 msb = result & (1 << 31);
        for (int i = 0; i < offset; ++i)
        {
            result >>= 1;
            result |= msb;
        }
    }
    // Special case LSR #32 / #0
    else
    {
        // Store the MSB in the carry
        carry = result >> 31;
        // Apply carry bit to whole result
        result = carry ? 0xFFFFFFFF : 0;
    }
    return carry;
}

// Rotate Right
u8 ARM::ROR(u32& result, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        for (int i = 0; i < offset; ++i)
        {
            carry = result & 0b1;
            result >>= 1;
            result |= (carry << 31);
        }
    }
    // Special case ROR #0
    else
    {
        // Save the first bit in the carry
        carry = result & 0b1;
        // Rotate by one
        result >>= 1;
        // Change MSB to current carry
        result |= (flagC() << 31);
    }
    return carry;
}
