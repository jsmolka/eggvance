#include "arm.h"

// Logical Shift Left
u32 ARM::LSL(u32 value, u8 offset, bool flags)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value << (offset - 1)) >> 31;

        value <<= offset;
    }
    // Special case LSL #0
    else
    {
        carry = flagC();
    }

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
        updateFlagC(carry);
    }

    return value;
}

// Logical Shift Right
u32 ARM::LSR(u32 value, u8 offset, bool flags)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0x1;

        value >>= offset;
    }
    // Special case LSR #0 / #32
    else
    {
        // Store the MSB in the carry
        carry = value >> 31;
        // Reset the result
        value = 0;
    }

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
        updateFlagC(carry);
    }

    return value;
}

// Arithmetic Shift Right
u32 ARM::ASR(u32 value, u8 offset, bool flags)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0x1;

        u32 msb = value & (1 << 31);
        for (int x = 0; x < offset; ++x)
        {
            value >>= 1;
            value |= msb;
        }
    }
    // Special case ASR #0 / #32
    else
    {
        // Store the MSB in the carry
        carry = value >> 31;
        // Apply carry bit to whole result
        value = carry ? 0xFFFFFFFF : 0;
    }

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
        updateFlagC(carry);
    }

    return value;
}

// Rotate Right
u32 ARM::ROR(u32 value, u8 offset, bool flags)
{
    u8 carry = 0;

    if (offset > 0)
    {
        // Todo: this can be optimized
        for (int x = 0; x < offset; ++x)
        {
            carry = value & 0x1;
            value >>= 1;
            value |= carry << 31;
        }
    }
    // Special case ROR #0 (RRX)
    else
    {
        // Save the first bit in the carry
        carry = value & 0x1;
        // Rotate by one
        value >>= 1;
        // Change MSB to current carry
        value |= (flagC() << 31);
    }

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
        updateFlagC(carry);
    }

    return value;
}

// Add
u32 ARM::ADD(u32 value, u32 operand, bool flags)
{
    u32 result = value + operand;

    if (flags)
    {
        updateFlagZ(result);
        updateFlagN(result);
        updateFlagC(value, operand, true);
        updateFlagV(value, operand, true);
    }

    return result;
}

// Subtract
u32 ARM::SUB(u32 value, u32 operand, bool flags)
{
    u32 result = value - operand;

    if (flags)
    {
        updateFlagZ(result);
        updateFlagN(result);
        updateFlagC(value, operand, false);
        updateFlagV(value, operand, false);
    }

    return result;
}

// Add with carry
u32 ARM::ADC(u32 value, u32 operand, bool flags)
{
    return ADD(value, operand + flagC(), flags);
}

// Subtract with carry
u32 ARM::SBC(u32 value, u32 operand, bool flags)
{
    return SUB(value, operand + (flagC() ? 0 : 1), flags);
}

// Multiply
u32 ARM::MUL(u32 value, u32 operand, bool flags)
{
    value *= operand;

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
    }

    return value;
}

// And
u32 ARM::AND(u32 value, u32 operand, bool flags)
{
    value &= operand;

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
    }

    return value;
}

// Or
u32 ARM::ORR(u32 value, u32 operand, bool flags)
{
    value |= operand;

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
    }

    return value;

}

// Exclusive or
u32 ARM::EOR(u32 value, u32 operand, bool flags)
{
    value ^= operand;

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
    }

    return value;
}

// Bit clear
u32 ARM::BIC(u32 value, u32 operand, bool flags)
{
    value &= ~operand;

    if (flags)
    {
        updateFlagZ(value);
        updateFlagN(value);
    }

    return value;
}

// Negative
u32 ARM::NEG(u32 operand, bool flags)
{
    return SUB(0, operand, flags);
}

// Move
u32 ARM::MOV(u32 operand, bool flags)
{
    if (flags)
    {
        updateFlagZ(operand);
        updateFlagN(operand);
    }

    return operand;
}

// Move negative
u32 ARM::MVN(u32 operand, bool flags)
{
    operand = ~operand;

    if (flags)
    {
        updateFlagZ(operand);
        updateFlagN(operand);
    }

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

// Load sign extended half
u32 ARM::LDSH(u32 addr)
{
    u32 half = mmu->readHalf(addr);

    // Extend with bit 16
    if (half & (1 << 15))
        half |= 0xFFFF0000;

    return half;
}

// Load sign extended byte
u32 ARM::LDSB(u32 addr)
{
    u32 byte = mmu->readByte(addr);

    // Extend with bit 8
    if (byte & (1 << 7))
        byte |= 0xFFFFFF00;

    return byte;
}

// Store multiple, increment after
u32 ARM::STMIA(u32 addr, u8 rlist)
{
    for (int x = 0; x < 8; ++x)
    {
        if (rlist & 0x1)
        {
            mmu->writeWord(addr, reg(x));
            addr += 4;
        }
        rlist >>= 1;
    }
    return addr;
}

// Load multiple, increment after
u32 ARM::LDMIA(u32 addr, u8 rlist)
{
    for (int x = 0; x < 8; ++x)
    {
        if (rlist & 0x1)
        {
            reg(x) = mmu->readWord(addr);
            addr += 4;
        }
        rlist >>= 1;
    }
    return addr;
}

// Push registers onto the stack
void ARM::PUSH(u8 rlist, bool lr)
{
    u32& sp = reg(13);

    // Store LR
    if (lr)
    {
        sp -= 4;
        mmu->writeWord(sp, reg(14));
    }

    // Iterate over specified registers
    for (int x = 7; x >= 0; --x)
    {
        if (rlist & (1 << x))
        {
            sp -= 4;
            mmu->writeWord(sp, reg(x));
        }
    }
}

// Pop registers from the stack
void ARM::POP(u8 rlist, bool pc)
{
    u32& sp = reg(13);

    // Iterate over specified registers
    for (int x = 0; x < 8; ++x)
    {
        if (rlist & 0x1)
        {
            reg(x) = mmu->readWord(sp);
            sp += 4;
        }
        rlist >>= 1;
    }

    // Load PC
    if (pc)
    {
        regs.r15 = mmu->readWord(sp);
        regs.r15 &= ~0x1;

        sp += 4;

        needs_flush = true;
    }
}

// Branch with exchange
void ARM::BX(u32 value)
{
    // Todo: is this correct?

    if (value & 0x0)
    {
        // Switch to ARM mode
        regs.cpsr &= ~CPSR_T;

        // Align word
        value &= ~(1 << 1);
    }

    regs.r15 = value;
    needs_flush = true;
}
