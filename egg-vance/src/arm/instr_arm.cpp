#include "arm.h"

/**
 * Todo
 * - ARM 1: using PC as operand
 * - ARM 3: flush if PC is PC (in sub, add)?
 * - ARM 4: rework and write proper test (SPSR and mode switching)
 * - ARM 11: sign extension still needed with new code?
 */

#include "common/log.h"
#include "common/utility.h"

u32 ARM::rotatedImmediate(u16 value, bool& carry)
{
    // Immediate 8-bit value
    u8 immediate = value & 0xFF;
    // Rotation applied to the immediate value
    u8 rotation = value >> 8 & 0xF;

    // RRX does not exist in this case
    if (rotation == 0)
    {
        // Keep current carry
        carry = regs.c();

        return immediate;
    }
    else
    {
        // Twice the rotation is applied
        return ror(immediate, 2 * rotation, carry);
    }
}

u32 ARM::shiftedRegister(u16 value, bool& carry)
{
    // 8-bit shift data
    u8 shift = value >> 4 & 0xFF;
    // Source register
    u8 rm = value & 0xF;

    u8 offset;
    if (shift & 0x1)
    {
        // Shift register
        u8 rs = shift >> 4 & 0xF;
        // Offset is stored in the lower byte
        offset = regs[rs] & 0xFF;

        if (rs == 15)
            log() << "Handle me";

        // Register shifts by zero are ignored (special cases use immediate)
        if (offset == 0)
        {
            // Keep current carry
            carry = regs.c();

            return regs[rm];
        }
    }
    else
    {
        // Offset is a 5-bit immediate value
        offset = shift >> 3 & 0x1F;
    }

    switch (shift >> 1 & 0x3)
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
    // Operand register
    u8 rn = instr & 0xF;

    // Undefined for PC
    if (rn == 15)
        log() << "Handle error";

    u32 addr = regs[rn];

    if (addr & 0x1)
    {
        // Switch to thumb
        regs.setThumb(true);
        align_half(addr);
    }
    else
    {
        align_word(addr);
    }

    regs.pc = addr;
    needs_flush = true;
}

// ARM 2
void ARM::branchLink(u32 instr)
{
    // Link flag
    bool link = instr >> 24 & 0x1;
    // 24-bit immediate value
    u32 offset = instr & 0xFFFFFF;

    s32 signed_offset = twos<24>(offset);

    // Shift left by two bits
    signed_offset <<= 2;

    if (link)
        // Save address of next instruction
        regs.lr = regs.pc - 4;

    regs.pc += signed_offset;
    needs_flush = true;
}

// ARM 3
void ARM::dataProcessing(u32 instr)
{
    // Immediate operand flag
    bool immediate = instr >> 25 & 0x1;
    // Operation code
    u8 opcode = instr >> 21 & 0xF;
    // Set conditions flag
    bool set_flags = instr >> 20 & 0x1;
    // First operand register
    u8 rn = instr >> 16 & 0xF;
    // Destination register
    u8 rd = instr >> 12 & 0xF;
    // Second operand data
    u32 op2 = instr & 0xFFF;

    u32 op1 = regs[rn];
    u32& dst = regs[rd];

    // Get second operand value
    bool carry;
    if (immediate)
        op2 = rotatedImmediate(op2, carry);
    else
        op2 = shiftedRegister(op2, carry);

    // Writing to PC
    if (set_flags && rd == 15)
    {
        if (regs.spsr)
            // Move current SPSR into CPSR
            regs.cpsr = *regs.spsr;
        else
            log() << "Handle me";

        // Do not set flags
        set_flags = 0;
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

        // Flush when modifying PC
        if (rd == 15)
            needs_flush = true;
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
}

// ARM 4
void ARM::psrTransfer(u32 instr)
{
    // MSR / MRS flag
    bool msr = instr >> 21 & 0x1;
    // SPSR / CPSR flag
    bool spsr = instr >> 22 & 0x1;

    if (spsr && !regs.spsr)
        log() << "Handle me";

    // MSR (register / immediate to PSR)
    if (msr)
    {
        // Immediate value / register flag
        bool immediate = instr >> 25 & 0x1;
        // Affect flag bits only
        bool flags = (instr >> 12 & 0x3FF) == 0b1010001111;

        u32 op;
        if (immediate)
        {
            bool carry;
            op = rotatedImmediate(instr, carry);
        }
        else
        {
            // Source register
            u8 rm = instr & 0xF;

            if (rm == 15)
                log() << "Handle me";

            op = regs[rm];
        }

        u32* dst;
        if (spsr)
        {
            dst = regs.spsr;
        }
        else
        {
            if (!flags)
            {
                if ((op & 0x1F) != (regs.cpsr & 0x1F))
                    // Switch mode if needed
                    regs.switchMode(static_cast<Mode>(op & 0x1F));
            }
            dst = &regs.cpsr;
        }

        if (flags)
        {
            *dst &= 0x0FFFFFFF;
            *dst |= op;
        }
        else
        {
            *dst = op;
        }
    }
    else  // MRS (PSR to register)
    {
        // Destination register
        u8 rd = instr >> 12 & 0xF;

        if (rd == 15)
            log() << "Handle me";

        regs[rd] = spsr ? *regs.spsr : regs.cpsr;
    }
}

// ARM 5
void ARM::multiply(u32 instr)
{
    // Accumulate flag
    bool accumulate = instr >> 21 & 0x1;
    // Set conditions flag
    bool set_flags = instr >> 20 & 0x1;
    // Destination register
    u8 rd = instr >> 16 & 0xF;
    // Operand registers
    u8 rn = instr >> 12 & 0xF;
    u8 rs = instr >> 8 & 0xF;
    u8 rm = instr & 0xF;

    if (rd == rm || rd == 15 || rm == 15)
        log() << "Handle me";

    u32& dst = regs[rd];
    u32 op1 = regs[rm];
    u32 op2 = regs[rs];

    // Multiply (MUL)
    dst = op1 * op2;

    // Accumulate (MLA)
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
