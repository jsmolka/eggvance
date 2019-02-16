#include "arm.h"

#include "common/log.h"

ARM::ARM()
{

}

void ARM::reset()
{
    regs = {};

    regs.r13 = 0x03007F00;
    regs.r13_fiq = 0x03007F00;
    regs.r13_abt = 0x03007F00;
    regs.r13_und = 0x03007F00;
    regs.r13_svc = 0x03007FE0;
    regs.r13_irq = 0x03007FA0;

    regs.pc() = 0x8000000;

    regs.cpsr = 0x5F;

    // For test ROM
    regs.cpsr |= CPSR_T;
    regs.pc() = 0x8000108;

    flushPipe();

    needs_flush = false;
    running = true;
}

void ARM::step()
{
    fetch();
    decode();
    execute();

    if (needs_flush)
    {
        flushPipe();

        needs_flush = false;
    }
    else
    {
        advance();
    }
}

u32& ARM::reg(u8 number)
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
        log() << "Tried accessing invalid register " << (int)number;
    }

    static u32 dummy = 0;
    return dummy;
}

u32& ARM::spsr(u8 number)
{
    switch (currentMode())
    {
    case MODE_FIQ: return regs.spsr_fiq;
    case MODE_SVC: return regs.spsr_svc;
    case MODE_ABT: return regs.spsr_abt;
    case MODE_IRQ: return regs.spsr_fiq;
    case MODE_UND: return regs.spsr_und;

    default:
        log() << "Tried accessing invalid spsr " << (int)number;
    }

    static u32 dummy = 0;
    return dummy;
}

void ARM::fetch()
{
    if (isArm())
        pipe[0].instr = mmu->readWord(regs.pc());
    else
        pipe[0].instr = mmu->readHalf(regs.pc());

    pipe[0].decoded = UNDEFINED;
}

void ARM::decode()
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
        else
        {
            log() << "Cannot decode ARM instruction " << (int)instr;
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
            log() << "Cannot decode THUMB instruction " << (int)instr;
        }
    }
}
 
void ARM::execute()
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
                branchExchange(instr);
                break;

            case ARM_11:
                branchLink(instr);
                break;

            default:
                log() << "Tried executing unknown THUMB instruction " << (int)pipe[2].decoded;
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
            addSubImmediate(instr);
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
            loadPcRelative(instr);
            break;

        case THUMB_7:
            loadStoreRegisterOffset(instr);
            break;

        case THUMB_8:
            loadStoreSignExtended(instr);
            break;

        case THUMB_9:
            loadStoreImmediateOffset(instr);
            break;

        case THUMB_10:
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
            log() << "Tried executing unknown THUMB instruction " << (int)pipe[2].decoded;
        }
    }
}

void ARM::advance()
{
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    regs.pc() += isThumb() ? 2 : 4;
}

void ARM::flushPipe()
{
    pipe[0] = { 0, REFILL_PIPE };
    pipe[1] = { 0, REFILL_PIPE };
    pipe[2] = { 0, REFILL_PIPE };
}

Mode ARM::currentMode() const
{
    return static_cast<Mode>(regs.cpsr & CPSR_M);
}

bool ARM::isArm() const
{
    return !isThumb();
}

bool ARM::isThumb() const
{
    return regs.cpsr & CPSR_T;
}

u8 ARM::flagZ() const
{
    return (regs.cpsr & CPSR_Z) ? 1 : 0;
}

u8 ARM::flagN() const
{
    return (regs.cpsr & CPSR_N) ? 1 : 0;
}

u8 ARM::flagC() const
{
    return (regs.cpsr & CPSR_C) ? 1 : 0;
}

u8 ARM::flagV() const
{
    return (regs.cpsr & CPSR_V) ? 1 : 0;
}

void ARM::setFlag(CPSR flag, bool set)
{
    if (set)
        regs.cpsr |= flag;
    else
        regs.cpsr &= ~flag;
}

void ARM::setFlagZ(bool set)
{
    setFlag(CPSR_Z, set);
}

void ARM::setFlagN(bool set)
{
    setFlag(CPSR_N, set);
}

void ARM::setFlagC(bool set)
{
    setFlag(CPSR_C, set);
}

void ARM::setFlagV(bool set)
{
    setFlag(CPSR_V, set);
}

void ARM::updateFlagZ(u32 res)
{
    setFlagZ(res == 0);
}

void ARM::updateFlagN(u32 res)
{
    setFlagN(res >> 31);
}

void ARM::updateFlagC(u8 carry)
{
    setFlagC(carry == 1);
}

void ARM::updateFlagC(u32 value, u32 operand, bool addition)
{
    bool carry;

    if (addition)
        carry = operand > (0xFFFFFFFF - value);
    else
        carry = operand > value;

    setFlagC(carry);
}

void ARM::updateFlagV(u32 value, u32 operand, bool addition)
{
    u8 msb_input = value >> 31;
    u8 msb_operand = operand >> 31;

    bool overflow = false;

    if (addition)
    {
        u8 msb_result = (value + operand) >> 31;
        if (msb_input == msb_operand)
            overflow = msb_result != msb_input;
    }
    else
    {
        u8 msb_result = (value - operand) >> 31;
        if (msb_input != msb_operand)
            overflow = msb_result == msb_input;
    }

    setFlagV(overflow);
}

bool ARM::checkCondition(Condition cond) const
{
    if (cond == COND_AL)
        return true;

    u8 z = flagZ();
    u8 n = flagN();
    u8 c = flagC();
    u8 v = flagV();

    switch (cond)
    {
    case COND_EQ: return z;
    case COND_NE: return !z;
    case COND_CS: return c;
    case COND_CC: return !c;
    case COND_MI: return n;
    case COND_PL: return !n;
    case COND_VS: return v;
    case COND_VC: return !v;
    case COND_HI: return c && !z;
    case COND_LS: return !c || z;
    case COND_GE: return n == v;
    case COND_LT: return n != v;
    case COND_GT: return !z && (n == v);
    case COND_LE: return z || (n != v);
    case COND_AL: return true;
    case COND_NV: return false;

    default:
        log() << "Invalid condition " << (int)cond;
    }
    return true;
}

bool ARM::checkBranchCondition(Condition cond) const
{
    if (cond == COND_AL)
    {
        log() << "Undefined branch condition AL";
        return false;
    }
    if (cond == COND_NV)
    {
        // Todo: process SWI
        return false;
    }
    return checkCondition(cond);
}
