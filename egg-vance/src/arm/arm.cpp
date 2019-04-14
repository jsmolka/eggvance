#include "arm.h"

/**
 * Todo:
 * - optimizie ROR
 * - ARM runtime exception
 * - should there be multiple elifs for coprocessor instructions in arm?
 * - check arm condition in decode to increase performance
 */

#include "common/log.h"
#include "common/utility.h"
#include "decoder.h"
#include "disassembler.h"

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
    debug();
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
    if (regs.isArm())
        pipe[0].instr = mmu->readWord(regs.pc);
    else
        pipe[0].instr = mmu->readHalf(regs.pc);

    pipe[0].format = FMT_NONE;
}

void ARM::decode()
{
    if (pipe[1].format == FMT_PIPE)
        return;

    pipe[1].format = Decoder::decode(pipe[1].instr, regs.isArm());
}
 
void ARM::execute()
{
    if (pipe[2].format == FMT_PIPE)
        return; 

    if (!regs.isThumb())
    {
        u32 instr = pipe[2].instr;

        Condition condition = static_cast<Condition>(instr >> 28);

        if (regs.checkCondition(condition))
        {
            switch (pipe[2].format)
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
                log() << "Unimplemented instruction " << (int)pipe[2].format;
                break;

            default:
                log() << "Unknown ARM instruction " << (int)pipe[2].format;
            }
        }
    }
    else
    {
        u16 instr = static_cast<u16>(pipe[2].instr);

        switch (pipe[2].format)
        {
        case THUMB_1:  
            moveShiftedRegister(instr);        
            break;

        case THUMB_2:
            addSubImmediate(instr);            
            break;

        case THUMB_3:
            addSubMovCmpImmediate(instr);     
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
            softwareInterruptBreakpoint(instr);     
            break;

        case THUMB_18:
            unconditionalBranch(instr);        
            break;

        case THUMB_19:
            longBranchLink(instr);             
            break;

        default:
            log() << "Unknown THUMB instruction " << (int)pipe[2].format;
        }
    }
}

void ARM::advance()
{
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    regs.pc += regs.isThumb() ? 2 : 4;
}

void ARM::debug()
{
    if (pipe[2].format == FMT_PIPE)
        return;

    std::cout << hex(regs.pc - 4) << "  " << Disassembler::disassemble(pipe[2].instr, pipe[2].format) << "\n";
}

void ARM::flush()
{
    pipe[0] = { 0, FMT_PIPE };
    pipe[1] = { 0, FMT_PIPE };
    pipe[2] = { 0, FMT_PIPE };
}

void ARM::updateZ(u32 result)
{
    regs.setZ(result == 0);
}

void ARM::updateN(u32 result)
{
    regs.setN(result >> 31);
}

void ARM::updateC(u32 op1, u32 op2, bool addition)
{
    bool carry;

    if (addition)
        carry = op2 > (0xFFFFFFFF - op1);
    else
        carry = op2 <= op1;

    regs.setC(carry);
}

void ARM::updateV(u32 op1, u32 op2, bool addition)
{
    int msb_op1 = op1 >> 31;
    int msb_op2 = op2 >> 31;

    bool overflow = false;

    if (addition)
    {
        int msb_result = (op1 + op2) >> 31;
        if (msb_op1 == msb_op2)
            overflow = msb_result != msb_op1;
    }
    else
    {
        int msb_result = (op1 - op2) >> 31;
        if (msb_op1 != msb_op2)
            overflow = msb_result == msb_op2;
    }

    regs.setV(overflow);
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

u32 ARM::lsl(u32 value, int offset, bool& carry)
{
    if (offset != 0)
    {
        if (offset < 32)
        {
            carry = value >> (32 - offset) & 0x1;
            value <<= offset;
        }
        else  // Shifts by 32 are undefined behavior in C++
        {
            if (offset == 32)
                carry = value & 0x1;
            else
                carry = 0;

            value = 0;
        }
    }
    else  // Special case LSL #0
    {
        carry = regs.c();
    }
    return value;
}

u32 ARM::lsr(u32 value, int offset, bool& carry)
{
    if (offset != 0 && offset != 32)
    {
        if (offset < 32)
        {
            carry = value >> (offset - 1) & 0x1;
            value >>= offset;
        }
        else
        {
            carry = 0;
            value = 0;
        }
    }
    else  // Special case LSR #32 (assembles to LSR #0)
    {
        carry = value >> 31;
        value = 0;
    }
    return value;
}

u32 ARM::asr(u32 value, int offset, bool& carry)
{
    if (offset != 0 && offset < 32)
    {
        bool msb = value >> 31;

        carry = value >> (offset - 1) & 0x1;
        value >>= offset;

        if (msb)
            value |= 0xFFFFFFFF << (31 - offset);
    }
    else  // Special case ASR #32 (assembles to ASR #0)
    {
        carry = value >> 31;
        value = carry ? 0xFFFFFFFF : 0;
    }
    return value;
}

u32 ARM::ror(u32 value, int offset, bool& carry)
{
    if (offset > 0)
    {
        offset %= 32;

        if (offset != 0)
            value = value << (32 - offset) | value >> offset;

        carry = value >> 31;
    }
    else  // Special case ROR #0 (RRX)
    {
        carry = value & 0x1;
        value >>= 1;
        value |= regs.c() << 31;
    }
    return value;
}

u32 ARM::ldr(u32 addr)
{
    u32 value;
    if (misalignedWord(addr))
    {
        bool carry;
        int rotation = (addr & 0x3) << 3;
        value = mmu->readWord(alignWord(addr));
        value = ror(value, rotation, carry);
    }
    else
    {
        value = mmu->readWord(addr);
    }
    return value;
}

u32 ARM::ldrh(u32 addr)
{
    u32 value;
    if (misalignedHalf(addr))
    {
        bool carry;
        value = mmu->readHalf(alignHalf(addr));
        value = ror(value, 8, carry);
    }
    else
    {
        value = mmu->readHalf(addr);
    }
    return value;
}

u32 ARM::ldrsh(u32 addr)
{
    u32 value;
    if (misalignedHalf(addr))
    {
        value = mmu->readByte(addr);
        if (value & (1 << 7))
            value |= 0xFFFFFF00;
    }
    else
    {
        value = mmu->readHalf(addr);
        if (value & (1 << 15))
            value |= 0xFFFF0000;
    }
    return value;
}
