#include "arm.h"

/**
 * Todo
 * - ARM 11: sign extension still needed with new code?
 */

#include "common/log.h"
#include "common/utility.h"

u32 ARM::rotatedImmediate(int value, bool& carry)
{
    int imm8 = value & 0xFF;
    int rotation = (value >> 8) & 0xF;

    // No RRX in this case
    if (rotation == 0)
    {
        carry = regs.c();
        return imm8;
    }
    else
    {
        // Apply twice the rotation
        return ror(imm8, 2 * rotation, carry);
    }
}

u32 ARM::shiftedRegister(int value, bool& carry)
{
    int shift = (value >> 4) & 0xFF;
    int rm = value & 0xF;

    int offset;
    if (shift & 0x1)
    {
        int rs = (shift >> 4) & 0xF;
        // Offset is stored in the lower byte
        offset = regs[rs] & 0xFF;

        // No RRX in this case
        if (offset == 0)
        {
            carry = regs.c();
            return regs[rm];
        }
    }
    else
    {
        // Offset is a 5-bit immediate value
        offset = (shift >> 3) & 0x1F;
    }

    int type = (shift >> 1) & 0x3;
    switch (type)
    {
    case 0b00: return lsl(regs[rm], offset, carry);
    case 0b01: return lsr(regs[rm], offset, carry);
    case 0b10: return asr(regs[rm], offset, carry);
    case 0b11: return ror(regs[rm], offset, carry);

    default:
        // Just for you, Visual Studio
        return regs[rm];
    }
}

 // ARM 1
void ARM::branchExchange(u32 instr)
{
    int rn = instr & 0xF;

    u32 addr = regs[rn];

    if (addr & 0x1)
    {
        addr = alignHalf(addr);
        // Change instruction set
        regs.setThumb(true);
    }
    else
    {
        addr = alignWord(addr);
    }

    regs.pc = addr;
    needs_flush = true;
}

// ARM 2
void ARM::branchLink(u32 instr)
{
    bool link = (instr >> 24) & 0x1;
    // 24-bit immediate value
    int imm24 = instr & 0xFFFFFF;

    int offset = twos<24>(imm24);

    // Offset is a 26-bit constant
    offset <<= 2;

    if (link)
        regs.lr = regs.pc - 4;

    regs.pc += offset;
    needs_flush = true;
}

// ARM 3
void ARM::dataProcessing(u32 instr)
{
    bool use_imm = (instr >> 25) & 0x1;
    int opcode = (instr >> 21) & 0xF;
    bool set_flags = (instr >> 20) & 0x1;
    int rn = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;
    u32 op2 = instr & 0xFFF;

    u32& dst = regs[rd];
    u32 op1 = regs[rn];

    bool carry;
    if (use_imm)
        op2 = rotatedImmediate(op2, carry);
    else
        op2 = shiftedRegister(op2, carry);

    // Writing to PC
    if (set_flags && rd == 15)
    {
        // Switch mode
        u32 spsr = regs.spsr;
        regs.switchMode(static_cast<Mode>(spsr));
        regs.cpsr = spsr;

        set_flags = false;
    }

    switch (opcode)
    {
    // AND
    case 0b0000:
        dst = op1 & op2;
        if (set_flags) 
            logical(dst, carry);
        break;

    // EOR
    case 0b0001:
        dst = op1 ^ op2;
        if (set_flags) 
            logical(dst, carry);
        break;

    // SUB
    case 0b0010:
        dst = op1 - op2;
        if (set_flags) 
            arithmetic(op1, op2, false);
        break;

    // RSB
    case 0b0011:
        dst = op2 - op1;
        if (set_flags) 
            arithmetic(op2, op1, false);
        break;

    // ADD
    case 0b0100:
        dst = op1 + op2;
        if (set_flags) 
            arithmetic(op1, op2, true);
        break;

    // ADC
    case 0b0101:
        op2 += regs.c();
        dst = op1 + op2;
        if (set_flags) 
            arithmetic(op1, op2, true);
        break;

    // SBC
    case 0b0110:
        op2 += regs.c() ? 0 : 1;
        dst = op1 - op2;
        if (set_flags) 
            arithmetic(op1, op2, false);
        break;

    // RBC
    case 0b0111:
        op1 += regs.c() ? 0 : 1;
        dst = op2 - op1;
        if (set_flags) 
            arithmetic(op2, op1, false);
        break;

    // TST
    case 0b1000:
        logical(op1 & op2, carry);
        break;

    // TEQ
    case 0b1001:
        logical(op1 ^ op2, carry);
        break;

    // CMP
    case 0b1010:
        arithmetic(op1, op2, false);
        break;

    // CMN
    case 0b1011:
        arithmetic(op1, op2, true);
        break;

    // ORR
    case 0b1100:
        dst = op1 | op2;
        if (set_flags) 
            logical(dst, carry);
        break;

    // MOV
    case 0b1101:
        dst = op2;
        if (set_flags) 
            logical(dst, carry);
        break;

    // BIC
    case 0b1110:
        dst = op1 & ~op2;
        if (set_flags) 
            logical(dst, carry);
        break;

    // MVN
    case 0b1111:
        dst = ~op2;
        if (set_flags) 
            logical(dst, carry);
        break;
    }

    if (rd == 15)
    {
        dst = alignWord(dst);
        needs_flush = true;
    }
}

// ARM 4
void ARM::psrTransfer(u32 instr)
{
    bool msr = (instr >> 21) & 0x1;
    bool use_spsr = (instr >> 22) & 0x1;

    // MSR
    if (msr)
    {
        bool use_imm = (instr >> 25) & 0x1;

        u32 op;
        if (use_imm)
        {
            bool carry;
            op = rotatedImmediate(instr & 0xFFF, carry);
        }
        else
        {
            int rm = instr & 0xF;
            op = regs[rm];
        }

        // Create mask based on fsxc-bits
        u32 mask = 0;
        if (instr & (1 << 16))
            mask |= 0x000000FF;
        if (instr & (1 << 17))
            mask |= 0x0000FF00;
        if (instr & (1 << 18))
            mask |= 0x00FF0000;
        if (instr & (1 << 19))
            mask |= 0xFF000000;

        op &= mask;

        if (use_spsr)
        {
            regs.spsr = (regs.spsr & ~mask) | op;
        }
        else
        {
            // Switch mode if control bit is set
            if (mask & 0xFF)
                regs.switchMode(static_cast<Mode>(op & 0x1F));

            regs.cpsr = (regs.cpsr & ~mask) | op;

            // Technically undefined behavior
            if (regs.isThumb())
            {
                regs.cpsr = alignHalf(regs.cpsr);
                needs_flush = true;
            }
        }
    }
    else  // MRS
    {
        int rd = (instr >> 12) & 0xF;
        regs[rd] = use_spsr ? regs.spsr : regs.cpsr;
    }
}

// ARM 5
void ARM::multiply(u32 instr)
{
    bool accumulate = (instr >> 21) & 0x1;
    bool set_flags = (instr >> 20) & 0x1;
    int rd = (instr >> 16) & 0xF;
    int rn = (instr >> 12) & 0xF;
    int rs = (instr >> 8) & 0xF;
    int rm = instr & 0xF;

    u32& dst = regs[rd];

    dst = regs[rs] * regs[rm];

    if (accumulate) 
        dst += regs[rn];

    if (set_flags)
        logical(dst);
}

// ARM 6
void ARM::multiplyLong(u32 instr)
{
    // Signed / unsigned flag
    bool sign = instr >> 22 & 0x1;
    // Accumulate flag
    bool accumulate = instr >> 21 & 0x1;
    // Set conditions flag
    bool set_flags = instr >> 20 & 0x1;
    // Source / destination registers
    u8 rdhi = instr >> 16 & 0xF;
    u8 rdlo = instr >> 12 & 0xF;
    // Operand registers
    u8 rs = instr >> 8 & 0xF;
    u8 rm = instr & 0xF;

    if (rs == 15 || rm == 15 || rdhi == 15 
        || rdlo == 15 || rdlo == rdhi || rdlo == rm)
        log() << "Handle me";

    u32& dsthi = regs[rdhi];
    u32& dstlo = regs[rdlo];
    u64 op1 = regs[rm];
    u64 op2 = regs[rs];

    if (sign)
    {
        // Sign extend to 64 bits if negative two's complement
        if (op1 & 1 << 31) 
            op1 |= 0xFFFFFFFF00000000;
        if (op2 & 1 << 31) 
            op2 |= 0xFFFFFFFF00000000;
    }
    
    // Multiply (UMULL / SMULL)
    u64 result = op1 * op2;

    // Accumulate (UMLAL / SMLAL)
    if (accumulate)
        result += static_cast<u64>(dsthi) << 32 | dstlo;

    if (set_flags)
    {
        regs.setZ(result == 0);
        regs.setN(result >> 63);
    }

    dsthi = result >> 32;
    dstlo = result & 0xFFFFFFFF;
}

// ARM 7
void ARM::singleDataTransfer(u32 instr)
{
    // Todo: is address aligned???
    // Todo: shifted register by 0

    // Register / immediate flag
    bool use_reg = instr >> 25 & 0x1;
    // Pre- / post-indexing flag
    bool pre_indexing = instr >> 24 & 0x1;
    // Up / down flag
    bool up = instr >> 23 & 0x1;
    // Byte / word flag
    int byte = instr >> 22 & 0x1;
    // Writeback flag
    bool writeback = instr >> 21 & 0x1;
    // Load / store flag
    int load = instr >> 20 & 0x1;
    // Base register
    u8 rn = instr >> 16 & 0xF;
    // Source / destination register
    u8 rd = instr >> 12 & 0xF;
    // 12-bit immediate value / shifted register
    u32 offset = instr & 0xFFF;

    if (rn == 15 && writeback)
        // Todo: handle PC as shift offset rm
        log() << "Handle error";

    // Offset is a shifted register
    if (use_reg)
    {
        bool carry;
        // Register shift amounts are not available in this instruction
        offset = shiftedRegister(offset, carry);
    }

    // Post-indexing always writes back (but writeback is 0)
    writeback |= !pre_indexing;

    // Get base address
    u32 addr = regs[rn];
    // Get destination register
    u32& dst = regs[rd];

    // Pre-indexing
    if (pre_indexing)
    {
        if (up)
            addr += offset;
        else
            addr -= offset;
    }

    switch (load << 1 | byte)
    {
    case 0b00:
        // STR
        // Stored value will be address of instruction + 12 (8 already because of pipe)
        mmu->writeWord(addr & ~0x3, dst + ((rd == 15) ? 4 : 0));
        break;

    case 0b01:
        // STRB
        mmu->writeByte(addr & ~0x3, dst & 0xFF);
        break;

    case 0b10:
        // LDR
        dst = mmu->readWord(addr & ~0x3);
        break;

    case 0b11:
        // LDRB
        dst = mmu->readByte(addr & ~0x3);
        break;
    }

    // Post-indexing
    if (!pre_indexing)
    {
        if (up)
            addr += offset;
        else
            addr -= offset;
    }

    // Writeback address
    if (writeback)
        regs[rn] = addr;
}

// ARM 8
void ARM::halfSignedDataTransfer(u32 instr)
{
    // Pre- / post-indexing flag
    bool pre_indexing = instr >> 24 & 0x1;
    // Up / down flag
    bool up = instr >> 23 & 0x1;
    // Writeback flag
    bool writeback = instr >> 21 & 0x1;
    // Load / store flag
    bool load = instr >> 20 & 0x1;
    // Base register
    u8 rn = instr >> 16 & 0xF;
    // Source / destination register
    u8 rd = instr >> 12 & 0xF;
    // Signed / unsigned flag
    int sign = instr >> 6 & 0x1;
    // Halfword / byte flag
    int half = instr >> 5 & 0x1;

    if (rn == 15 && writeback)
        log() << "Handle error";

    u32 offset;
    if (instr >> 22 & 0x1)
    {
        // Immediate offset
        int upper = instr >> 8 & 0xF;
        int lower = instr & 0xF;
        offset = upper << 4 | lower;
    }
    else
    {
        // Register offset
        u8 rm = instr & 0xF;
        if (rm == 15)
            log() << "Handle error";
        offset = regs[rm];
    }

    // Post-indexing always writes back (but writeback is 0)
    writeback |= !pre_indexing;

    // Get base address
    u32 addr = regs[rn];
    // Get destination register
    u32& dst = regs[rd];

    // Pre-indexing
    if (pre_indexing)
    {
        if (up)
            addr += offset;
        else
            addr -= offset;
    }

    switch (sign << 1 | half)
    {
    // SWP
    case 0b00:
        log() << "Wrong instruction for SWP";
        break;

    // STRH / LDRH
    case 0b01:
        if (load)
            dst = mmu->readHalf(addr & ~0x1);
        else
            // Stored value will be address of instruction + 12 (8 already because of pipe)
            mmu->writeHalf(addr & ~0x1, dst + ((rn == 15) ? 4 : 0));
        break;

    // LDRSB
    case 0b10:
        dst = mmu->readByte(addr & ~0x1);
        if (dst & 1 << 7)
            dst |= 0xFFFFFF00;
        break;

    // LDRSH
    case 0b11:
        dst = mmu->readHalf(addr & ~0x1);
        if (dst & 1 << 15)
            dst |= 0xFFFF0000;
        break;
    }

    // Post-indexing
    if (!pre_indexing)
    {
        if (up)
            addr += offset;
        else
            addr -= offset;
    }

    if (writeback)
        regs[rn] = addr;
}

// ARM 9
void ARM::blockDataTransfer(u32 instr)
{
    // Full / empty stack
    bool full_stack = instr >> 24 & 0x1;
    // Ascending / descending stack
    bool ascending_stack = instr >> 23 & 0x1;
    // Load PSR and force user
    bool psr_user = instr >> 22 & 0x1;
    // Writeback flag
    bool writeback = instr >> 21 & 0x1;
    // Load / store flag
    bool load = instr >> 20 & 0x1;
    // Base register
    int rn = instr >> 16 & 0xF;
    // Register list
    int rlist = instr & 0xFFFF;

    if (rlist == 0 || rn == 15)
        log() << "Handle me";

    // Handle S bit
    Mode mode = regs.mode();

    if (psr_user)
        regs.switchMode(MODE_USR);

    // Base address
    u32 addr = regs[rn];

    // Lowest register gets stored at the lowest address
    if (ascending_stack)
    {
        // Start at lowest address, load / store registers in order
        for (int x = 0; x < 16; ++x)
        {
            if (rlist & 1 << x)
            {
                if (full_stack)
                    addr += 4;

                if (load)
                {
                    regs[x] = mmu->readWord(addr & ~0x3);

                    if (x == 15)
                        needs_flush = true;
                }
                else
                {
                    mmu->writeWord(addr & ~0x3, regs[x]);
                }
            
                if (!full_stack)
                    addr += 4;
            }
        }
    }
    else
    {
        // Start at highest address, load / store registers in reverse order
        for (int x = 15; x >= 0; --x)
        {
            if (rlist & 1 << x)
            {
                if (full_stack)
                    addr -= 4;

                if (load)
                {
                    regs[x] = mmu->readWord(addr & ~0x3);

                    if (x == 15)
                        needs_flush = true;
                }
                else
                {
                    mmu->writeWord(addr & ~0x3, regs[x]);
                }

                if (!full_stack)
                    addr -= 4;
            }
        }
    }

    if (writeback)
        regs[rn] = addr;

    if (psr_user)
        regs.switchMode(mode);
}

// ARM 10
void ARM::singleDataSwap(u32 instr)
{
    // Byte / word flag
    int byte = instr >> 22 & 0x1;
    // Base register
    int rb = instr >> 16 & 0xF;
    // Destination register
    int rd = instr >> 12 & 0xF;
    // Source register
    int rs = instr & 0xF;

    if (rb == 15 || rd == 15 || rs == 15)
        log() << "Handle error";

    u32 addr = regs[rb];
    u32& dst = regs[rd];
    u32 src = regs[rs];

    if (addr & 0x3)
    {
        // Todo: fix for misaligned addresses
    }

    switch (byte)
    {
    // SWPB
    case 0b1:
        dst = mmu->readByte(addr);
        mmu->writeByte(addr, src);
        break;

    // SWP
    case 0b0:
        dst = mmu->readWord(addr);
        mmu->writeWord(addr, src);
        break;
    }
}
