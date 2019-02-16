#include "arm.h"

// Logical Shift Left
u32 ARM::LSL(u32 value, u8 offset)
{
    u8 carry;

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
u32 ARM::LSR(u32 value, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0x1;

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
u32 ARM::ASR(u32 value, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0x1;

        u32 msb = value & (1 << 31);
        for (u8 i = 0; i < offset; ++i)
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
u32 ARM::ROR(u32 value, u8 offset)
{
    u8 carry = 0;

    if (offset > 0)
    {
        for (int i = 0; i < offset; ++i)
        {
            carry = value & 0x1;
            value >>= 1;
            value |= (carry << 31);
        }
    }
    // Special case ROR #0
    else
    {
        // Save the first bit in the carry
        carry = value & 0x1;
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

// Add
u32 ARM::ADD(u32 value, u32 operand)
{
    u32 result = value + operand;

    updateFlagZ(result);
    updateFlagN(result);
    updateFlagC(value, operand, true);
    updateFlagV(value, operand, true);

    return result;
}

// Subtract
u32 ARM::SUB(u32 value, u32 operand)
{
    u32 result = value - operand;

    updateFlagZ(result);
    updateFlagN(result);
    updateFlagC(value, operand, false);
    updateFlagV(value, operand, false);

    return result;
}

// Add with carry
u32 ARM::ADC(u32 value, u32 operand)
{
    return ADD(value, operand + flagC());
}

// Subtract with carry
u32 ARM::SBC(u32 value, u32 operand)
{
    return SUB(value, operand - 1 + flagC());
}

// Multiply
u32 ARM::MUL(u32 value, u32 operand)
{
    // Todo: check about C and V flag

    value *= operand;

    updateFlagZ(value);
    updateFlagN(value);

    return value;
}

// And
u32 ARM::AND(u32 value, u32 operand)
{
    value &= operand;

    updateFlagZ(value);
    updateFlagN(value);

    return value;
}

// Or
u32 ARM::ORR(u32 value, u32 operand)
{
    value |= operand;

    updateFlagZ(value);
    updateFlagN(value);

    return value;

}

// Exclusive or
u32 ARM::EOR(u32 value, u32 operand)
{
    value ^= operand;

    updateFlagZ(value);
    updateFlagN(value);

    return value;
}

// Negative
u32 ARM::NEG(u32 operand)
{
    u32 result = (0 - operand);

    updateFlagZ(result);
    updateFlagN(result);
    updateFlagC(0, operand, false);
    updateFlagV(0, operand, false);

    return result;
}

// Bit clear
u32 ARM::BIC(u32 value, u32 operand)
{
    value &= ~operand;

    updateFlagZ(value);
    updateFlagN(value);

    return value;
}

// Move
u32 ARM::MOV(u32 operand)
{
    updateFlagZ(operand);
    updateFlagN(operand);

    return operand;
}

// Move negative
u32 ARM::MVN(u32 operand)
{
    operand = ~operand;

    updateFlagZ(operand);
    updateFlagN(operand);

    return operand;
}

// Compare
void ARM::CMP(u32 value, u32 operand)
{
    SUB(value, operand);
}

// Compare negative
void ARM::CMN(u32 value, u32 operand)
{
    ADD(value, operand);
}

// Test bits
void ARM::TST(u32 value, u32 operand)
{
    AND(value, operand);
}

// Store word
void ARM::STRW(u32 addr, u32 value)
{
    mmu->writeWord(addr, value);
}

// Store half
void ARM::STRH(u32 addr, u32 value)
{
    mmu->writeHalf(addr, value & 0xFFFF);
}

// Store byte
void ARM::STRB(u32 addr, u32 value)
{
    mmu->writeByte(addr, value & 0xFF);
}

// Load word
u32 ARM::LDRW(u32 addr)
{
    return mmu->readWord(addr);
}

// Load half
u16 ARM::LDRH(u32 addr)
{
    return mmu->readHalf(addr);
}

// Load byte
u8 ARM::LDRB(u32 addr)
{
    return mmu->readByte(addr);
}
