#include "arm7.h"

#include <iostream>

ARM7::ARM7()
    : mmu(nullptr)
    , running(true)
{

}

void ARM7::reset()
{
    regs = {};

    regs.r13 = 0x03007F00;
    regs.r13_fiq = 0x03007F00;
    regs.r13_abt = 0x03007F00;
    regs.r13_und = 0x03007F00;
    regs.r13_svc = 0x03007FE0;
    regs.r13_irq = 0x03007FA0;

    regs.r15 = 0x8000000;

    regs.cpsr = 0x5F;

    // For test ROM
    regs.cpsr |= CPSR_T;
    regs.r15 = 0x8000108;


    pipe[0] = { 0, REFILL_PIPE };
    pipe[1] = { 0, REFILL_PIPE };
    pipe[2] = { 0, REFILL_PIPE };

    needs_flush = false;
    running = true;
}

u32 ARM7::reg(u8 number) const
{
    Mode mode = currentMode();

    switch (number)
    {
    case 0: return regs.r0;
    case 1: return regs.r1;
    case 2: return regs.r2;
    case 3: return regs.r3;
    case 4: return regs.r4;
    case 5: return regs.r5;
    case 6: return regs.r6;
    case 7: return regs.r7;
    case 8: return mode == MODE_FIQ ? regs.r8_fiq : regs.r8;
    case 9: return mode == MODE_FIQ ? regs.r9_fiq : regs.r9;
    case 10: return mode == MODE_FIQ ? regs.r10_fiq : regs.r10;
    case 11: return mode == MODE_FIQ ? regs.r11_fiq : regs.r11;
    case 12: return mode == MODE_FIQ ? regs.r12_fiq : regs.r12;

    case 13:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: return regs.r13;
        case MODE_FIQ: return regs.r13_fiq;
        case MODE_SVC: return regs.r13_svc;
        case MODE_ABT: return regs.r13_abt;
        case MODE_IRQ: return regs.r13_irq;
        case MODE_UND: return regs.r13_und;
        }

    case 14:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: return regs.r14;
        case MODE_FIQ: return regs.r14_fiq;
        case MODE_SVC: return regs.r14_svc;
        case MODE_ABT: return regs.r14_abt;
        case MODE_IRQ: return regs.r14_irq;
        case MODE_UND: return regs.r14_und;
        }

    case 15: return regs.r15;

    default:
        std::cout << __FUNCTION__ << " - Tried accessing invalid register " << (int)number << "\n";
    }
    return 0;
}

void ARM7::setReg(u8 number, u32 value)
{
    Mode mode = currentMode();

    switch (number)
    {
    case 0: regs.r0 = value; break;
    case 1: regs.r1 = value; break;
    case 2: regs.r2 = value; break;
    case 3: regs.r3 = value; break;
    case 4: regs.r4 = value; break;
    case 5: regs.r5 = value; break;
    case 6: regs.r6 = value; break;
    case 7: regs.r7 = value; break;
    case 8: (mode == MODE_FIQ ? regs.r8_fiq : regs.r8) = value; break;
    case 9: (mode == MODE_FIQ ? regs.r9_fiq : regs.r9) = value; break;
    case 10: (mode == MODE_FIQ ? regs.r10_fiq : regs.r10) = value; break;
    case 11: (mode == MODE_FIQ ? regs.r11_fiq : regs.r11) = value; break;
    case 12: (mode == MODE_FIQ ? regs.r12_fiq : regs.r12) = value; break;

    case 13:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: regs.r13 = value; break;
        case MODE_FIQ: regs.r13_fiq = value; break;
        case MODE_SVC: regs.r13_svc = value; break;
        case MODE_ABT: regs.r13_abt = value; break;
        case MODE_IRQ: regs.r13_irq = value; break;
        case MODE_UND: regs.r13_und = value; break;
        }
        break;

    case 14:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: regs.r14 = value; break;
        case MODE_FIQ: regs.r14_fiq = value; break;
        case MODE_SVC: regs.r14_svc = value; break;
        case MODE_ABT: regs.r14_abt = value; break;
        case MODE_IRQ: regs.r14_irq = value; break;
        case MODE_UND: regs.r14_und = value; break;
        }
        break;

    case 15: regs.r15 = value; break;

    default:
        std::cout << __FUNCTION__ << " - Tried setting invalid register " << (int)number << "\n";
    }
}

u32 ARM7::spsr(u8 number) const
{
    switch (currentMode())
    {
    case MODE_FIQ: return regs.spsr_fiq;
    case MODE_SVC: return regs.spsr_svc;
    case MODE_ABT: return regs.spsr_abt;
    case MODE_IRQ: return regs.spsr_fiq;
    case MODE_UND: return regs.spsr_und;

    default:
        std::cout << __FUNCTION__ << " - Tried accessing invalid spsr " << (int)number << "\n";
    }
    return 0;
}

void ARM7::setSpsr(u8 number, u32 value)
{
    switch (currentMode())
    {
    case MODE_FIQ: regs.spsr_fiq = value; break;
    case MODE_SVC: regs.spsr_svc = value; break;
    case MODE_ABT: regs.spsr_abt = value; break;
    case MODE_IRQ: regs.spsr_fiq = value; break;
    case MODE_UND: regs.spsr_und = value; break;

    default:
        std::cout << __FUNCTION__ << " - Tried setting invalid spsr " << (int)number << "\n";
    }
}

void ARM7::fetch()
{
    if (isArm())
        pipe[0].instr = mmu->readWord(regs.r15);
    else
        pipe[0].instr = mmu->readHalf(regs.r15);

    pipe[0].decoded = UNDEFINED;
}

void ARM7::decode()
{
    if (pipe[1].decoded == REFILL_PIPE)
        return;
    
    if (isArm())
    {
        u32 instr = pipe[1].instr;

        if ((instr >> 26 & 0x3) == 0b00)
        {
            if ((instr >> 22 & 0x3F) == 0b000000
                && (instr >> 4 & 0xF) == 0b1001)
            {
                // Multiply
                pipe[1].decoded = ARM_2;
            }
            else if ((instr >> 23 & 0x1F) == 0b00001
                && (instr >> 4 & 0xF) == 0b1001)
            {
                // Multiply long
                pipe[1].decoded = ARM_3;
            }
            else if ((instr >> 23 & 0x1F) == 0b00010
                && (instr >> 20 & 0x3) == 0b00
                && (instr >> 4 & 0xFF) == 0b00001001)
            {
                // Single data swap
                pipe[1].decoded = ARM_4;
            }
            else if ((instr >> 4 & 0xFFFFFF) == 0b000100101111111111110001)
            {
                // Branch and exchange
                pipe[1].decoded = ARM_5;
            }
            else if ((instr >> 25 & 0x7) == 0b000
                && (instr >> 22 & 0x1) == 0b0
                && (instr >> 7 & 0x1F) == 0b00001
                && (instr >> 4 & 0x1) == 0b1)
            {
                // Halfword data transfer (register offset)
                pipe[1].decoded = ARM_6;
            }
            else if ((instr >> 25 & 0x7) == 0b000
                && (instr >> 22 & 0x1) == 0b1
                && (instr >> 7 & 0x1) == 0b1
                && (instr >> 4 & 0x1) == 0b1)
            {
                // Halfword data transfer (immediate offset)
                pipe[1].decoded = ARM_7;
            }
            else
            {
                // Data processing / PSR transfer
                pipe[1].decoded = ARM_1;
            }
        }
        else if ((instr >> 25 & 0x7) == 0b011
            && (instr >> 4 & 0x1) == 0b1)
        {
            // Undefined
            pipe[1].decoded = ARM_9;
        }
        else if ((instr >> 25 & 0x3) == 0b01)
        {
            // Single data transfer
            pipe[1].decoded = ARM_8;
        }
        else if ((instr >> 25 & 0x7) == 0b100)
        {
            // Block data transfer
            pipe[1].decoded = ARM_10;
        }
        else if ((instr >> 25 & 0x7) == 0b101)
        {
            // Branch
            pipe[1].decoded = ARM_11;
        }
        else if ((instr >> 25 & 0x7) == 0b110)
        {
            // Coprocessor data transfer
            pipe[1].decoded = ARM_12;
        }
        else if ((instr >> 24 & 0xF) == 0b1110)
        {
            if ((instr >> 4 & 0x1) == 0b0)
            {
                // Coprocessor data operation
                pipe[1].decoded = ARM_13;
            }
            else
            {
                // Coprocessor register transfer
                pipe[1].decoded = ARM_14;
            }
        }
        else if ((instr >> 24 & 0xF) == 0b1111)
        {
            // Softrware interrupt
            pipe[1].decoded = ARM_15;
        }
    }
    else
    {
        u16 instr = static_cast<u16>(pipe[1].instr);

        if ((instr >> 11 & 0x1F) == 0b00011)
        {
            pipe[1].decoded = THUMB_2;
        }
        else if ((instr >> 13 & 0x7) == 0b000)
        {
            pipe[1].decoded = THUMB_1;
        }
        else if ((instr >> 13 & 0x7) == 0b001)
        {
            pipe[1].decoded = THUMB_3;
        }
        else if ((instr >> 10 & 0x3F) == 0b010000)
        {
            pipe[1].decoded = THUMB_4;
        }
        else if ((instr >> 10 & 0x3F) == 0b010001)
        {
            pipe[1].decoded = THUMB_5;
        }
        else if ((instr >> 11 & 0x1F) == 0b01001)
        {
            pipe[1].decoded = THUMB_6;
        }
        else if ((instr >> 12 & 0xF) == 0b0101)
        {
            pipe[1].decoded = ((instr >> 9 & 0x1) == 0b0) ? THUMB_7 : THUMB_8;
        }
        else if ((instr >> 13 & 0x7) == 0b011)
        {
            pipe[1].decoded = THUMB_9;
        }
        else if ((instr >> 12 & 0xF) == 0b1000)
        {
            pipe[1].decoded = THUMB_10;
        }
        else if ((instr >> 12 & 0xF) == 0b1001)
        {
            pipe[1].decoded = THUMB_11;
        }
        else if ((instr >> 12 & 0xF) == 0b1010)
        {
            pipe[1].decoded = THUMB_12;
        }
        else if ((instr >> 12 & 0xF) == 0b1011)
        {
            pipe[1].decoded = ((instr >> 10 & 0x1) == 0b0) ? THUMB_13 : THUMB_14;
        }
        else if ((instr >> 12 & 0xF) == 0b1100)
        {
            pipe[1].decoded = THUMB_15;
        }
        else if ((instr >> 12 & 0xF) == 0b1101)
        {
            pipe[1].decoded = ((instr >> 8 & 0xF) == 0b1111) ? THUMB_17 : THUMB_16;
        }
        else if ((instr >> 12 & 0xF) == 0b1110)
        {
            pipe[1].decoded = THUMB_18;
        }
        else if ((instr >> 12 & 0xF) == 0b1111)
        {
            pipe[1].decoded = THUMB_19;
        }
        else
        {
            std::cout << __FUNCTION__ << " - Cannot decode THUMB instruction " << (int)instr << "\n";
        }
    }
}
 
void ARM7::execute()
{
    if (pipe[2].decoded == REFILL_PIPE)
        return; 

    if (isArm())
    {
        u32 instr = pipe[2].instr;

        Condition condition = static_cast<Condition>(instr >> 28);

        if (checkCondition(condition))
        {
            switch (pipe[2].decoded)
            {
            case ARM_5:
                std::cout << "ARM_5\n";
                branchExchange(instr);
                break;

            case ARM_11:
                std::cout << "ARM_11\n";
                branchLink(instr);
                break;

            default:
                std::cout << __FUNCTION__ << " - Tried executing unknown thumb instruction " << (int)pipe[2].decoded << "\n";
            }
        }
    }
    else
    {
        u16 instr = static_cast<u16>(pipe[2].instr);

        switch (pipe[2].decoded)
        {
        case THUMB_1:
            std::cout << "THUMB_1\n";
            moveShiftedRegister(instr);
            break;

        case THUMB_2:
            std::cout << "THUMB_2\n";
            addSubImmediate(instr);
            break;

        case THUMB_3:
            std::cout << "THUMB_3\n";
            moveCmpAddSubImmediate(instr);
            break;

        case THUMB_4:
            aluOperations(instr);
            break;

        case THUMB_5:
            highRegisterBranchExchange(instr);
            break;

        case THUMB_6:
            loadPcRelative(instr);
            break;

        case THUMB_7:
            loadStoreRegisterOffset(instr);
            break;

        case THUMB_8:
            loadStoreSignExtended(instr);
            break;

        case THUMB_9:
            std::cout << "THUMB_9\n";
            loadStoreImmediateOffset(instr);
            break;

        case THUMB_10:
            std::cout << "THUMB_10\n";
            loadStoreHalfword(instr);
            break;

        case THUMB_11:
            loadStoreSpRelative(instr);
            break;

        case THUMB_12:
            loadAddress(instr);
            break;

        case THUMB_13:
            addOffsetSp(instr);
            break;

        case THUMB_14:
            pushPopRegisters(instr);
            break;

        case THUMB_15:
            multipleLoadStore(instr);
            break;

        case THUMB_16:
            std::cout << "THUMB_16\n";
            conditionalBranch(instr);
            break;

        case THUMB_17:
            softwareInterruptThumb(instr);
            break;

        case THUMB_18:
            unconditionalBranch(instr);
            break;

        case THUMB_19:
            longBranchLink(instr);
            break;

        default:
            std::cout << __FUNCTION__ << " - Tried executing unknown thumb instruction " << (int)pipe[2].decoded << "\n";
        }
    }
}

void ARM7::advance()
{
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    regs.r15 += isThumb() ? 2 : 4;
}

void ARM7::step()
{
    fetch();
    decode();
    execute();

    if (needs_flush)
    {
        pipe[0] = { 0, REFILL_PIPE };
        pipe[1] = { 0, REFILL_PIPE };
        pipe[2] = { 0, REFILL_PIPE };

        needs_flush = false;
    }
    else
    {
        advance();
    }
}

ARM7::Mode ARM7::currentMode() const
{
    return static_cast<Mode>(regs.cpsr & CPSR_M);
}

bool ARM7::isArm() const
{
    return !isThumb();
}

bool ARM7::isThumb() const
{
    return regs.cpsr & CPSR_T;
}

u8 ARM7::flagZ() const
{
    return (regs.cpsr & CPSR_Z) ? 1 : 0;
}

u8 ARM7::flagN() const
{
    return (regs.cpsr & CPSR_N) ? 1 : 0;
}

u8 ARM7::flagC() const
{
    return (regs.cpsr & CPSR_C) ? 1 : 0;
}

u8 ARM7::flagV() const
{
    return (regs.cpsr & CPSR_V) ? 1 : 0;
}

void ARM7::setFlag(CPSR flag, bool set)
{
    if (set)
        regs.cpsr |= flag;
    else
        regs.cpsr &= ~flag;
}

void ARM7::setFlagZ(bool set)
{
    setFlag(CPSR_Z, set);
}

void ARM7::setFlagN(bool set)
{
    setFlag(CPSR_N, set);
}

void ARM7::setFlagC(bool set)
{
    setFlag(CPSR_C, set);
}

void ARM7::setFlagV(bool set)
{
    setFlag(CPSR_V, set);
}

void ARM7::updateFlagZ(u32 res)
{
    setFlagZ(res == 0);
}

void ARM7::updateFlagN(u32 res)
{
    setFlagN(res >> 31);
}

void ARM7::updateFlagC(u8 carry)
{
    setFlagC(carry == 1);
}

void ARM7::updateFlagC(u32 input, u32 operand, bool addition)
{
    bool carry;

    if (addition)
        carry = operand > (0xFFFFFFFF - input);
    else
        carry = operand > input;

    setFlagC(carry);
}

void ARM7::updateFlagV(u32 input, u32 operand, bool addition)
{
    u8 msb_input = input >> 31;
    u8 msb_operand = operand >> 31;

    bool overflow = false;

    if (addition)
    {
        u8 msb_result = (input + operand) >> 31;
        if (msb_input == msb_operand)
            overflow = msb_result != msb_input;
    }
    else
    {
        u8 msb_result = (input - operand) >> 31;
        if (msb_input != msb_operand)
            overflow = msb_result == msb_input;
    }

    setFlagV(overflow);
}

u8 ARM7::logicalShiftLeft(u32& result, u8 offset)
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

u8 ARM7::logicalShiftRight(u32& result, u8 offset)
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

u8 ARM7::arithmeticShiftRight(u32& result, u8 offset)
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

u8 ARM7::rotateRight(u32& result, u8 offset)
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

bool ARM7::checkCondition(Condition condition) const
{
    if (condition == COND_AL)
        return true;

    u8 z = flagZ();
    u8 n = flagN();
    u8 c = flagC();
    u8 v = flagV();

    switch (condition)
    {
    // EQ - Z set
    case COND_EQ: 
        return z;

    // NE - Z clear
    case COND_NE:
        return !z;

    // CS - C set
    case COND_CS:
        return c;

    // CC - C clear
    case COND_CC:
        return !c;

    // MI - N set
    case COND_MI:
        return n;

    // PL - N clear
    case COND_PL:
        return !n;

    // VS - V set
    case COND_VS:
        return v;

    // VC - V clear
    case COND_VC:
        return !v;

    // HI - C set and Z clear
    case COND_HI:
        return c && !z;

    // LS - C clear or Z set
    case COND_LS:
        return !c || z;

    // GE - N equals V
    case COND_GE:
        return n == v;

    // LT - N not equal to V
    case COND_LT:
        return n != v;

    // GT - Z clear and (N equals V)
    case COND_GT:
        return !z && (n == v);

    // LE - Z set or (N not equal to V)
    case COND_LE:
        return z || (n != v);

    // AL - always true
    case COND_AL:
        return true;

    // NV - never true
    case COND_NV:
        return false;

    default:
        std::cout << __FUNCTION__ << " - Invalid condition " << (int)condition << "\n";
    }
    return true;
}