#include "arm7.h"

#include <iostream>

ARM7::ARM7()
    : mmu(nullptr)
{

}

void ARM7::reset()
{
    pipe[0] = 0;
    pipe[1] = 0;
    pipe[2] = 0;

    pipe_instr[0] = REFILL_PIPE;
    pipe_instr[1] = REFILL_PIPE;
    pipe_instr[2] = REFILL_PIPE;
}

u32 ARM7::reg(u8 number) const
{
    Mode mode = static_cast<Mode>(regs.cpsr & CPSR_M);

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
    Mode mode = static_cast<Mode>(regs.cpsr & CPSR_M);

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
    switch (static_cast<Mode>(regs.cpsr & CPSR_M))
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
    switch (static_cast<Mode>(regs.cpsr & CPSR_M))
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
    if (regs.cpsr & CPSR_T)
        pipe[0] = mmu->readHalf(regs.r15);
    else
        pipe[0] = mmu->readWord(regs.r15);

    pipe_instr[0] = UNDEFINED;
}

void ARM7::decode()
{
    if (pipe_instr[1] == REFILL_PIPE)
        return;

    if (regs.cpsr & CPSR_T)
    {
        u16 instr = static_cast<u16>(pipe[1]);

        if ((instr >> 11) == 0b00011)
        {
            pipe_instr[1] = THUMB_2;
        }
        else if ((instr >> 13) == 0b000)
        {
            pipe_instr[1] = THUMB_1;
        }
        else if ((instr >> 13) == 0b001)
        {
            pipe_instr[1] = THUMB_3;
        }
        else if ((instr >> 10) == 0b010000)
        {
            pipe_instr[1] = THUMB_4;
        }
        else if ((instr >> 10) == 0b010001)
        {
            pipe_instr[1] = THUMB_5;
        }
        else if ((instr >> 11) == 0b01001)
        {
            pipe_instr[1] = THUMB_6;
        }
        else if ((instr >> 12) == 0b0101)
        {
            if ((instr >> 9) & 0b1)
                pipe_instr[1] = THUMB_8;
            else
                pipe_instr[1] = THUMB_7;
        }
        else if ((instr >> 13) == 0b011)
        {
            pipe_instr[1] = THUMB_9;
        }
        else if ((instr >> 12) == 0b1000)
        {
            pipe_instr[1] = THUMB_10;
        }
        else if ((instr >> 12) == 0b1001)
        {
            pipe_instr[1] = THUMB_11;
        }
        else if ((instr >> 12) == 0b1010)
        {
            pipe_instr[1] = THUMB_12;
        }
        else if ((instr >> 12) == 0b1011)
        {
            if ((instr >> 10) & 0b1)
                pipe_instr[1] = THUMB_14;
            else
                pipe_instr[1] = THUMB_13;
        }
        else if ((instr >> 12) == 0b1100)
        {
            pipe_instr[1] = THUMB_15;
        }
        else if ((instr >> 12) == 0b1101)
        {
            if (((instr >> 8) & 0b1111) == 0b1111)
                pipe_instr[1] = THUMB_17;
            else
                pipe_instr[1] = THUMB_16;
        }
        else if ((instr >> 12) == 0b1110)
        {
            pipe_instr[1] = THUMB_18;
        }
        else if ((instr >> 12) == 0b1111)
        {
            pipe_instr[1] = THUMB_19;
        }
        else
        {
            std::cout << __FUNCTION__ << " - Cannot decode THUMB instruction " << (int)instr << "\n";
        }
    }
    else
    {
        // Todo: Decode ARM
    }
}
 
void ARM7::execute()
{
    if (pipe_instr[2] == REFILL_PIPE)
        return; 

    if (regs.cpsr & CPSR_T)
    {
        u16 instr = static_cast<u16>(pipe[2]);

        switch (pipe_instr[2])
        {
        case THUMB_1:
            moveShiftedRegister(instr);
            break;

        case THUMB_2:
            addSubtract(instr);
            break;

        case THUMB_3:
            moveCompareAddSubtractAddImmediate(instr);
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
            std::cout << __FUNCTION__ << " - Tried executing unknown instruction " << (int)pipe_instr[2] << "\n";
        }
    }
    else
    {
        // Todo: Execute ARM
    }
}

void ARM7::advance()
{
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    pipe_instr[2] = pipe_instr[1];
    pipe_instr[1] = pipe_instr[0];
}

void ARM7::step()
{
    fetch();
    decode();
    execute();
    advance();

    // Todo: check if this is correct
    // Advance PC depending on state
    regs.r15 += (regs.cpsr & CPSR_T) ? 2 : 4;
}

void ARM7::updateZero(u32 result)
{
    if (result == 0)
        regs.cpsr |= CPSR_Z;
    else
        regs.cpsr &= ~CPSR_Z;
}

void ARM7::updateSign(u32 result)
{
    if (result >> 31)
        regs.cpsr |= CPSR_N;
    else
        regs.cpsr &= ~CPSR_N;
}

void ARM7::updateCarry(bool carry)
{
    if (carry)
        regs.cpsr |= CPSR_C;
    else
        regs.cpsr &= ~CPSR_C;
}

void ARM7::updateOverflow(u32 value, u32 operand, u32 result, bool addition)
{
    u8 msb_value = value >> 31;
    u8 msb_operand = operand >> 31;
    u8 msb_result = result >> 31;

    bool overflown = false;
    if (addition)
    {
        if (msb_value == msb_operand)
            overflown = msb_result != msb_operand;
    }
    else
    {
        if (msb_value != msb_operand)
            overflown = msb_result == msb_operand;
    }

    if (overflown)
        regs.cpsr |= CPSR_V;
    else
        regs.cpsr &= ~CPSR_V;
}

u8 ARM7::logicalShiftLeft(u32& value, u8 offset)
{
    // Save the last bit shifted out in the carry
    u8 carry = (value << (offset - 1)) >> 31;

    value <<= offset;

    return carry;
}

u8 ARM7::logicalShiftRight(u32& value, u8 offset)
{
    // Save the last bit shifted out in the carry
    u8 carry = value >> (offset - 1);

    value >>= offset;

    return carry;
}

u8 ARM7::arithmeticShiftRight(u32& value, u8 offset)
{
    // Save the last bit shifted out in the carry
    u8 carry = value >> (offset - 1);

    u32 msb = value & (1 << 31);
    for (int i = 0; i < offset; ++i)
    {
        value >>= 1;
        value |= msb;
    }
    return carry;
}
