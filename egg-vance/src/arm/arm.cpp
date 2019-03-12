#include "arm.h"

#include "common/log.h"

void ARM::reset()
{
    regs.reset();

    flushPipe();

    needs_flush = false;
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

void ARM::fetch()
{
    if (!regs.isThumb())
        pipe[0].instr = mmu->readWord(regs.pc);
    else
        pipe[0].instr = mmu->readHalf(regs.pc);

    pipe[0].decoded = UNDEFINED;
}

void ARM::decode()
{
    if (pipe[1].decoded == REFILL_PIPE)
        return;
    
    if (!regs.isThumb())
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

    if (!regs.isThumb())
    {
        u32 instr = pipe[2].instr;

        Condition condition = static_cast<Condition>(instr >> 28);

        if (regs.checkCondition(condition))
        {
            switch (pipe[2].decoded)
            {
            case ARM_1:
                dataProcessing(instr);
                break;

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

        log() << "THUMB " << (int)pipe[2].decoded - 16;

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
            loadStoreHalfSignExtended(instr);      
            break;

        case THUMB_9:
            loadStoreImmediateOffset(instr);   
            break;

        case THUMB_10:
            loadStoreHalf(instr);          
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
            loadStoreMultiple(instr);          
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

    regs.pc += regs.isThumb() ? 2 : 4;
}

void ARM::flushPipe()
{
    pipe[0] = { 0, REFILL_PIPE };
    pipe[1] = { 0, REFILL_PIPE };
    pipe[2] = { 0, REFILL_PIPE };
}

void ARM::updateZ(u32 value)
{
    regs.setZ(value == 0);
}

void ARM::updateN(u32 value)
{
    regs.setN(value >> 31);
}

void ARM::updateC(u8 carry)
{
    regs.setC(carry == 1);
}

void ARM::updateC(u32 value, u32 operand, bool addition)
{
    bool carry;

    if (addition)
        carry = operand > (0xFFFFFFFF - value);
    else
        carry = operand <= value;

    regs.setC(carry);
}

void ARM::updateV(u32 value, u32 operand, bool addition)
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
            overflow = msb_result == msb_operand;
    }

    regs.setV(overflow);
}
