#include "arm7.h"

#include <iostream>

ARM7::ARM7()
    : mmu(nullptr)
{

}

void ARM7::reset()
{
    pipe[0] = { 0, REFILL_PIPE };
    pipe[1] = { 0, REFILL_PIPE };
    pipe[2] = { 0, REFILL_PIPE };

    needs_flush = false;
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

        if ((instr >> 25) & 0b001)
        {
            pipe[1].decoded = ARM_1;
        }
        else if ((instr >> 23) & 0b00000)
        {
            pipe[1].decoded = ARM_2;
        }
        else if ((instr >> 23) & 0b00001)
        {
            pipe[1].decoded = ARM_3;
        }
        else if ((instr >> 23) & 0b00010)
        {
            pipe[1].decoded = ((instr >> 31) & 0b0) ? ARM_4 : ARM_5;
        }
        else if ((instr >> 25) & 0b000)
        {
            pipe[1].decoded = ((instr >> 22) & 0b0) ? ARM_6 : ARM_7;
        }
    }
    else
    {
        u16 instr = static_cast<u16>(pipe[1].instr);

        if ((instr >> 11) & 0b00011)
        {
            pipe[1].decoded = THUMB_2;
        }
        else if ((instr >> 13) & 0b000)
        {
            pipe[1].decoded = THUMB_1;
        }
        else if ((instr >> 13) & 0b001)
        {
            pipe[1].decoded = THUMB_3;
        }
        else if ((instr >> 10) & 0b010000)
        {
            pipe[1].decoded = THUMB_4;
        }
        else if ((instr >> 10) & 0b010001)
        {
            pipe[1].decoded = THUMB_5;
        }
        else if ((instr >> 11) & 0b01001)
        {
            pipe[1].decoded = THUMB_6;
        }
        else if ((instr >> 12) & 0b0101)
        {
            pipe[1].decoded = ((instr >> 9) & 0b0) ? THUMB_7 : THUMB_8;
        }
        else if ((instr >> 13) & 0b011)
        {
            pipe[1].decoded = THUMB_9;
        }
        else if ((instr >> 12) & 0b1000)
        {
            pipe[1].decoded = THUMB_10;
        }
        else if ((instr >> 12) & 0b1001)
        {
            pipe[1].decoded = THUMB_11;
        }
        else if ((instr >> 12) & 0b1010)
        {
            pipe[1].decoded = THUMB_12;
        }
        else if ((instr >> 12) & 0b1011)
        {
            pipe[1].decoded = ((instr >> 10) & 0b0) ? THUMB_13 : THUMB_14;
        }
        else if ((instr >> 12) & 0b1100)
        {
            pipe[1].decoded = THUMB_15;
        }
        else if ((instr >> 12) & 0b1101)
        {
            pipe[1].decoded = ((instr >> 8) & 0b1111) ? THUMB_17 : THUMB_16;
        }
        else if ((instr >> 12) & 0b1110)
        {
            pipe[1].decoded = THUMB_18;
        }
        else if ((instr >> 12) & 0b1111)
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
            case ARM_1:
                dataProcessingPsrTransfer(instr);
                break;

            case ARM_2:
                multiply(instr);
                break;
        
            case ARM_3:
                multiplyLong(instr);
                break;

            case ARM_4:
                singleDataSwap(instr);
                break;

            case ARM_5:
                branchAndExchange(instr);
                break;

            case ARM_6:
                halfDataTransferRegisterOffset(instr);
                break;

            case ARM_7:
                halfDataTransferImmediateOffset(instr);
                break;

            case ARM_8:
                singleDataTransfer(instr);
                break;

            case ARM_9:
                undefined(instr);
                break;

            case ARM_10:
                blockDataTransfer(instr);
                break;

            case ARM_11:
                branch(instr);
                break;

            case ARM_12:
                coprocessorDataTransfer(instr);
                break;

            case ARM_13:
                coprocessorDataOperation(instr);
                break;

            case ARM_14:
                coprocessorRegisterTransfer(instr);
                break;

            case ARM_15:
                softwareInterrupt(instr);
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
            moveShiftedRegister(instr);
            break;

        case THUMB_2:
            addSubtract(instr);
            break;

        case THUMB_3:
            moveCmpAddSubImmediate(instr);
            break;

        case THUMB_4:
            aluOperations(instr);
            break;

        case THUMB_5:
            highRegisterBranchExchange(instr);
            break;

        case THUMB_6:
            pcRelativeLoad(instr);
            break;

        case THUMB_7:
            loadStoreWithRegisterOffset(instr);
            break;

        case THUMB_8:
            loadStoreSignExtendedByteHalfword(instr);
            break;

        case THUMB_9:
            loadStoreWithImmediateOffset(instr);
            break;

        case THUMB_10:
            loadStoreHalfword(instr);
            break;

        case THUMB_11:
            spRelativeLoadStore(instr);
            break;

        case THUMB_12:
            loadAddress(instr);
            break;

        case THUMB_13:
            addOffsetToSp(instr);
            break;

        case THUMB_14:
            pushPopRegisters(instr);
            break;

        case THUMB_15:
            multipleLoadStore(instr);
            break;

        case THUMB_16:
            conditionalBranch(instr);
            break;

        case THUMB_17:
            softwareInterrupt(instr);
            break;

        case THUMB_18:
            unconditionalBranch(instr);
            break;

        case THUMB_19:
            longBranchWithLink(instr);
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
    advance();

    if (needs_flush)
    {
        pipe[0] = { 0, REFILL_PIPE };
        pipe[1] = { 0, REFILL_PIPE };
        pipe[2] = { 0, REFILL_PIPE };

        needs_flush = false;
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

u8 ARM7::logicalShiftLeft(u32& value, u8 offset)
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
        // Todo: "the shifter carry out is the old value of the CPSR C flag"?
        carry = flagC();
    }
    return carry;
}

u8 ARM7::logicalShiftRight(u32& value, u8 offset)
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
    return carry;
}

u8 ARM7::arithmeticShiftRight(u32& value, u8 offset)
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
    return carry;
}

u8 ARM7::rotateRight(u32 &value, u8 offset)
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

    default:
        std::cout << __FUNCTION__ << " - Invalid condition " << (int)condition << "\n";
    }
    return true;
}