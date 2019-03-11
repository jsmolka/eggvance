#include "arm.h"

/**
 * Todo
 * - align in BX
 */

#include "common/utility.h"

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
        carry = regs.c();
    }

    if (flags)
    {
        updateZ(value);
        updateN(value);
        updateC(carry);
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
        updateZ(value);
        updateN(value);
        updateC(carry);
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
        updateZ(value);
        updateN(value);
        updateC(carry);
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
        value |= (regs.c() << 31);
    }

    if (flags)
    {
        updateZ(value);
        updateN(value);
        updateC(carry);
    }

    return value;
}

// Add
u32 ARM::ADD(u32 value, u32 operand, bool flags)
{
    u32 result = value + operand;

    if (flags)
    {
        updateZ(result);
        updateN(result);
        updateC(value, operand, true);
        updateV(value, operand, true);
    }

    return result;
}

// Subtract
u32 ARM::SUB(u32 value, u32 operand, bool flags)
{
    u32 result = value - operand;

    if (flags)
    {
        updateZ(result);
        updateN(result);
        updateC(value, operand, false);
        updateV(value, operand, false);
    }

    return result;
}

// Add with carry
u32 ARM::ADC(u32 value, u32 operand, bool flags)
{
    return ADD(value, operand + regs.c(), flags);
}

// Subtract with carry
u32 ARM::SBC(u32 value, u32 operand, bool flags)
{
    return SUB(value, operand + (regs.c() ? 0 : 1), flags);
}

// Multiply
u32 ARM::MUL(u32 value, u32 operand, bool flags)
{
    value *= operand;

    if (flags)
    {
        updateZ(value);
        updateN(value);
    }

    return value;
}

// And
u32 ARM::AND(u32 value, u32 operand, bool flags)
{
    value &= operand;

    if (flags)
    {
        updateZ(value);
        updateN(value);
    }

    return value;
}

// Or
u32 ARM::ORR(u32 value, u32 operand, bool flags)
{
    value |= operand;

    if (flags)
    {
        updateZ(value);
        updateN(value);
    }

    return value;

}

// Exclusive or
u32 ARM::EOR(u32 value, u32 operand, bool flags)
{
    value ^= operand;

    if (flags)
    {
        updateZ(value);
        updateN(value);
    }

    return value;
}

// Bit clear
u32 ARM::BIC(u32 value, u32 operand, bool flags)
{
    value &= ~operand;

    if (flags)
    {
        updateZ(value);
        updateN(value);
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
        updateZ(operand);
        updateN(operand);
    }

    return operand;
}

// Move negative
u32 ARM::MVN(u32 operand, bool flags)
{
    operand = ~operand;

    if (flags)
    {
        updateZ(operand);
        updateN(operand);
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
void ARM::STR(u32 addr, u32 value)
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
u32 ARM::LDR(u32 addr)
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
            mmu->writeWord(addr, regs[x]);
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
            regs[x] = mmu->readWord(addr);
            addr += 4;
        }
        rlist >>= 1;
    }
    return addr;
}

// Push registers onto the stack
void ARM::PUSH(u8 rlist, bool lr)
{
    // Store LR
    if (lr)
    {
        regs.sp -= 4;
        mmu->writeWord(regs.sp, regs.lr);
    }

    // Iterate over specified registers
    for (int x = 7; x >= 0; --x)
    {
        if (rlist & (1 << x))
        {
            regs.sp -= 4;
            mmu->writeWord(regs.sp, regs[x]);
        }
    }
}

// Pop registers from the stack
void ARM::POP(u8 rlist, bool pc)
{
    // Iterate over specified registers
    for (int x = 0; x < 8; ++x)
    {
        if (rlist & 0x1)
        {
            regs[x] = mmu->readWord(regs.sp);
            regs.sp += 4;
        }
        rlist >>= 1;
    }

    // Load PC
    if (pc)
    {
        regs.pc = mmu->readWord(regs.sp);

        align16(regs.pc);

        regs.sp += 4;

        needs_flush = true;
    }
}

// Branch with exchange
void ARM::BX(u32 value)
{
    if (value & 0x0)
    {
        // Switch to ARM mode
        regs.cpsr &= ~CPSR_T;

        // Align word
        value &= ~(1 << 1);
    }

    regs.pc = value;
    needs_flush = true;
}
