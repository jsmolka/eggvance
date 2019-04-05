#include "arm.h"

/**
 * Todo:
 * - optimizie ROR
 * - ARM runtime exception
 * - should there be multiple elifs for coprocessor instructions in arm?
 * - check arm condition in decode to increase performance
 */

#include "common/log.h"

void ARM::reset()
{
    regs.reset();

    flush();

    needs_flush = false;
}

void ARM::step()
{
    fetch();
    decode();
    execute();

    if (needs_flush)
    {
        flush();

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

        if ((instr >> 25 & 0x7) == 0b101)
        {
            pipe[1].decoded = ARM_2;  // Branch and branch with link
        }
        else if ((instr >> 25 & 0x7) == 0b100)
        {
            pipe[1].decoded = ARM_9;  // Block data transfer
        }
        else if ((instr >> 26 & 0x3) == 0b11)
        {
            pipe[1].decoded = ARM_11;  // Software interrupt

            // Could also be coprocessor instruction, but the GBA has none
        }
        else if ((instr >> 26 & 0x3) == 0b01)
        {
            pipe[1].decoded = ARM_7;  // Single data transfer

            // Could also be the undefined instruction, but seems linked to the coprocessor
        }
        else  // (instr >> 26 & 0x3) == 0b00
        {
            if ((instr >> 4 & 0xFFFFFF) == 0b000100101111111111110001)
            {
                pipe[1].decoded = ARM_1;  // Branch and exchange
            }
            else if ((instr >> 22 & 0xF) == 0b0000
                && (instr >> 4 & 0xF) == 0b1001)
            {
                pipe[1].decoded = ARM_5;  // Multiply and multiply-accumulate
            }
            else if ((instr >> 23 & 0x7) == 0b001
                && (instr >> 4 & 0xF) == 0b1001)
            {
                pipe[1].decoded = ARM_6;  // Multiply long and multiply-accumulate long
            }
            else if ((instr >> 23 & 0x7) == 0b010
                && (instr >> 20 & 0x3) == 0b00
                && (instr >> 4 & 0xFF) == 0b00001001)
            {
                pipe[1].decoded = ARM_10;  // Single data swap
            }
            else if ((instr >> 25 & 0x1) == 0b0
                && (instr >> 7 & 0x1) == 0b1
                && (instr >> 4 & 0x1) == 0b1)
            {
                pipe[1].decoded = ARM_8;  // Halfword data transfer
            }
            else
            {
                u8 opcode = instr >> 21 & 0xF;

                switch (opcode)
                {
                case 0b1000:  // TST
                case 0b1001:  // TEQ
                case 0b1010:  // CMP
                case 0b1011:  // CMN
                {
                    bool set_flags = instr >> 20 & 0x1;

                    if (set_flags)
                        pipe[1].decoded = ARM_3;  // Data processing
                    else
                        pipe[1].decoded = ARM_4;  // PSR transfer
                    break;
                }

                default:
                    pipe[1].decoded = ARM_3;  // Data processing
                }
            }
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

        log() << "ARM " << (int)pipe[2].decoded - 1;

        if (regs.checkCondition(condition))
        {
            switch (pipe[2].decoded)
            {
            case ARM_1:
                branchExchange(instr);
                break;

            case ARM_2:
                branchLink(instr);
                break;

            case ARM_3:
                dataProcessing(instr);
                break;

            case ARM_4:
                psrTransfer(instr);
                break;

            case ARM_5:
                multiply(instr);
                break;

            case ARM_6:
                multiplyLong(instr);
                break;

            case ARM_7:
                singleDataTransfer(instr);
                break;

            case ARM_8:
                halfSignedDataTransfer(instr);
                break;

            case ARM_9:
                blockDataTransfer(instr);
                break;

            case ARM_10:
                singleDataSwap(instr);
                break;

            case ARM_11:
            case ARM_12:
            case ARM_13:
            case ARM_14:
            case ARM_15:
                log() << "Unimplemented instruction " << (int)pipe[2].decoded;
                break;

            default:
                log() << "Unknown ARM instruction " << (int)pipe[2].decoded;
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
            log() << "Unknown THUMB instruction " << (int)pipe[2].decoded;
        }
    }
}

void ARM::advance()
{
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    regs.pc += regs.isThumb() ? 2 : 4;
}

void ARM::flush()
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
    u8 msb_value = value >> 31;
    u8 msb_operand = operand >> 31;

    bool overflow = false;

    if (addition)
    {
        u8 msb_result = (value + operand) >> 31;
        if (msb_value == msb_operand)
            overflow = msb_result != msb_value;
    }
    else
    {
        u8 msb_result = (value - operand) >> 31;
        if (msb_value != msb_operand)
            overflow = msb_result == msb_operand;
    }

    regs.setV(overflow);
}

void ARM::arithmetic(u32 op1, u32 op2, bool addition)
{
    u32 result = addition 
        ? op1 + op2 
        : op1 - op2;

    updateZ(result);
    updateN(result);
    updateC(op1, op2, addition);
    updateV(op1, op2, addition);
}

void ARM::logical(u32 result)
{
    updateZ(result);
    updateN(result);
}

void ARM::logical(u32 result, bool carry)
{
    updateZ(result);
    updateN(result);

    regs.setC(carry);
}

u32 ARM::lsl(u32 value, u8 offset, bool& carry)
{
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
    return value;
}

u32 ARM::lsr(u32 value, u8 offset, bool& carry)
{
    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0x1;

        value >>= offset;
    }
    // Todo: should this happen for #0 or #32?
    // Special case LSR #32
    else
    {
        // Store the MSB in the carry
        carry = value >> 31;
        // Reset the result
        value = 0;
    }
    return value;
}

u32 ARM::asr(u32 value, u8 offset, bool& carry)
{
    if (offset > 0)
    {
        // Save the last bit shifted out in the carry
        carry = (value >> (offset - 1)) & 0x1;

        // Todo: this can be optimized
        u32 msb = value & (1 << 31);
        for (int x = 0; x < offset; ++x)
        {
            value >>= 1;
            value |= msb;
        }
    }
    // Todo: should this happen for #0 or #32?
    // Special case ASR #32
    else
    {
        // Store the MSB in the carry
        carry = value >> 31;
        // Apply carry bit to whole result
        value = carry ? 0xFFFFFFFF : 0;
    }
    return value;
}

u32 ARM::ror(u32 value, u8 offset, bool& carry)
{
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
    return value;
}
