#include "arm.h"

void ARM::fetch()
{
    if (regs.arm())
        pipe[0].instr = mmu.readWord(regs.pc);
    else
        pipe[0].instr = mmu.readHalf(regs.pc);

    pipe[0].format = FMT_NONE;
}

void ARM::decode()
{
    if (pipe[1].format == FMT_REFILL)
        return;

    if (regs.arm())
    {
        u32 instr = pipe[1].instr;

        if (((instr >> 25) & 0x7) == 0b101)
        {
            pipe[1].format = ARM_2;
        }
        else if (((instr >> 25) & 0x7) == 0b100)
        {
            pipe[1].format = ARM_9;
        }
        else if (((instr >> 26) & 0x3) == 0b11)
        {
            // Could also be coprocessor instruction, but the GBA has none

            pipe[1].format = ARM_11;
        }
        else if (((instr >> 26) & 0x3) == 0b01)
        {
            // Could also be the undefined instruction

            pipe[1].format = ARM_7;
        }
        else
        {
            if (((instr >> 4) & 0xFFFFFF) == 0b000100101111111111110001)
            {
                pipe[1].format = ARM_1;
            }
            else if (((instr >> 22) & 0xF) == 0b0000 && ((instr >> 4) & 0xF) == 0b1001)
            {
                pipe[1].format = ARM_5;
            }
            else if (((instr >> 23) & 0x7) == 0b001 && ((instr >> 4) & 0xF) == 0b1001)
            {
                pipe[1].format = ARM_6;
            }
            else if (((instr >> 23) & 0x7) == 0b010 && ((instr >> 20) & 0x3) == 0b00 && ((instr >> 4) & 0xFF) == 0b00001001)
            {
                pipe[1].format = ARM_10;
            }
            else if (((instr >> 25) & 0x1) == 0b0 && ((instr >> 7) & 0x1) == 0b1 && ((instr >> 4) & 0x1) == 0b1)
            {
                pipe[1].format = ARM_8;
            }
            else
            {
                int opcode = (instr >> 21) & 0xF;
                switch (opcode)
                {
                case 0b1000:  // TST
                case 0b1001:  // TEQ
                case 0b1010:  // CMP
                case 0b1011:  // CMN
                {
                    int flags = (instr >> 20) & 0x1;
                    if (flags)
                        pipe[1].format = ARM_3;
                    else
                        pipe[1].format = ARM_4;
                    break;
                }

                default:
                    pipe[1].format = ARM_3;
                }
            }
        }
    }
    else
    {
        u16 instr = static_cast<u16>(pipe[1].instr);

        if (((instr >> 11) & 0x1F) == 0b00011)
        {
            pipe[1].format = THUMB_2;
        }
        else if (((instr >> 13) & 0x7) == 0b000)
        {
            pipe[1].format = THUMB_1;
        }
        else if (((instr >> 13) & 0x7) == 0b001)
        {
            pipe[1].format = THUMB_3;
        }
        else if (((instr >> 10) & 0x3F) == 0b010000)
        {
            pipe[1].format = THUMB_4;
        }
        else if (((instr >> 10) & 0x3F) == 0b010001)
        {
            pipe[1].format = THUMB_5;
        }
        else if (((instr >> 11) & 0x1F) == 0b01001)
        {
            pipe[1].format = THUMB_6;
        }
        else if (((instr >> 12) & 0xF) == 0b0101)
        {
            pipe[1].format = (((instr >> 9) & 0x1) == 0b0) ? THUMB_7 : THUMB_8;
        }
        else if (((instr >> 13) & 0x7) == 0b011)
        {
            pipe[1].format = THUMB_9;
        }
        else if (((instr >> 12) & 0xF) == 0b1000)
        {
            pipe[1].format = THUMB_10;
        }
        else if (((instr >> 12) & 0xF) == 0b1001)
        {
            pipe[1].format = THUMB_11;
        }
        else if (((instr >> 12) & 0xF) == 0b1010)
        {
            pipe[1].format = THUMB_12;
        }
        else if (((instr >> 12) & 0xF) == 0b1011)
        {
            pipe[1].format = (((instr >> 10) & 0x1) == 0b0) ? THUMB_13 : THUMB_14;
        }
        else if (((instr >> 12) & 0xF) == 0b1100)
        {
            pipe[1].format = THUMB_15;
        }
        else if (((instr >> 12) & 0xF) == 0b1101)
        {
            pipe[1].format = (((instr >> 8) & 0xF) == 0b1111) ? THUMB_17 : THUMB_16;
        }
        else if (((instr >> 12) & 0xF) == 0b1110)
        {
            pipe[1].format = THUMB_18;
        }
        else if (((instr >> 12) & 0xF) == 0b1111)
        {
            pipe[1].format = THUMB_19;
        }
    }
}

void ARM::execute()
{
    if (pipe[2].format == FMT_REFILL)
        return;

    if (regs.arm())
    {
        u32 instr = pipe[2].instr;

        if (regs.check(static_cast<Condition>(instr >> 28)))
        {
            switch (pipe[2].format)
            {
            case ARM_1:  branchExchange(instr); break;
            case ARM_2:  branchLink(instr); break;
            case ARM_3:  dataProcessing(instr); break;
            case ARM_4:  psrTransfer(instr); break;
            case ARM_5:  multiply(instr); break;
            case ARM_6:  multiplyLong(instr); break;
            case ARM_7:  singleTransfer(instr); break;
            case ARM_8:  halfSignedTransfer(instr); break;
            case ARM_9:  blockTransfer(instr); break;
            case ARM_10: singleSwap(instr); break;
            case ARM_11: swiArm(instr); break;
            }
        }
        else
        {
            cycle(regs.pc + 4, SEQ);
        }
    }
    else
    {
        u16 instr = static_cast<u16>(pipe[2].instr);

        switch (pipe[2].format)
        {
        case THUMB_1:  moveShiftedRegister(instr); break;
        case THUMB_2:  addSubImmediate(instr); break;
        case THUMB_3:  addSubMovCmpImmediate(instr); break;
        case THUMB_4:  aluOperations(instr); break;
        case THUMB_5:  highRegisterBranchExchange(instr); break;
        case THUMB_6:  loadPcRelative(instr); break;
        case THUMB_7:  loadStoreRegisterOffset(instr); break;
        case THUMB_8:  loadStoreHalfSigned(instr); break;
        case THUMB_9:  loadStoreImmediateOffset(instr); break;
        case THUMB_10: loadStoreHalf(instr); break;
        case THUMB_11: loadStoreSpRelative(instr); break;
        case THUMB_12: loadAddress(instr); break;
        case THUMB_13: addOffsetSp(instr); break;
        case THUMB_14: pushPopRegisters(instr); break;
        case THUMB_15: loadStoreMultiple(instr); break;
        case THUMB_16: conditionalBranch(instr); break;
        case THUMB_17: swiThumb(instr); break;
        case THUMB_18: unconditionalBranch(instr); break;
        case THUMB_19: longBranchLink(instr); break;
        }
    }
}

void ARM::advance()
{
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    regs.pc += regs.arm() ? 4 : 2;
}
