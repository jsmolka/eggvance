#include "arm.h"

/**
 * Todo
 * - PC + 12 in transfers
 * - Block Transfer might not be 100% corrent (with base register position in rlist)
 * - PSR access rights in different modes
 * - empty register list
 */

#include <iostream>

#include "utility.h"

u32 ARM::rotatedImmediate(int data, bool& carry)
{
    int rotation = (data >> 8) & 0xF;
    int immediate = data & 0xFF;

    // Apply twice the rotation
    return ror(immediate, 2 * rotation, carry, false);
}

u32 ARM::shiftedRegister(int data, bool& carry)
{
    int shift = (data >> 4) & 0xFF;
    int rm = data & 0xF;

    int value = regs[rm];

    int offset;
    bool immediate = (shift & 0x1) == 0b0;
    if (immediate)
    {
        // Offset is a 5-bit immediate value
        offset = (shift >> 3) & 0x1F;
    }
    else
    {
        int rs = (shift >> 4) & 0xF;
        // Offset is stored in the lower byte
        offset = regs[rs] & 0xFF;
    }

    int type = (shift >> 1) & 0x3;
    switch (type)
    {
    case 0b00: return lsl(value, offset, carry);
    case 0b01: return lsr(value, offset, carry, immediate);
    case 0b10: return asr(value, offset, carry, immediate);
    case 0b11: return ror(value, offset, carry, immediate);

    default:
        // Just for you, Visual Studio
        return value;
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
        regs.setThumb(true);
    }
    else
    {
        addr = alignWord(addr);
    }

	// 1N cycle
    cycle(regs.pc, true);

    regs.pc = addr;
    needs_flush = true;

	// 2S cycles
    cycle(regs.pc, false);
    cycle(regs.pc + 4, false);
}

// ARM 2
void ARM::branchLink(u32 instr)
{
    bool link = (instr >> 24) & 0x1;
    int offset = instr & 0xFFFFFF;

    offset = twos<24>(offset);
    offset <<= 2;

    if (link)
        regs.lr = regs.pc - 4;

	// 1N cycle
    cycle(regs.pc, true);

    regs.pc += offset;
    needs_flush = true;

	// 2S cycles
    cycle(regs.pc, false);
    cycle(regs.pc + 4, false);
}

// ARM 3
void ARM::dataProcessing(u32 instr)
{
    bool immediate = (instr >> 25) & 0x1;
    int opcode = (instr >> 21) & 0xF;
    bool flags = (instr >> 20) & 0x1;
    int rn = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;

    u32& dst = regs[rd];
    u32 op1 = regs[rn];

    u32 op2;
    bool carry;
    if (immediate)
    {
        op2 = rotatedImmediate(instr & 0xFFF, carry);
    }
    else
    {
        int shift = (instr >> 4) & 0xFF;
        int rm = instr & 0xF;

		int value = regs[rm];

        int offset;
        bool use_reg = shift & 0x1;
        if (use_reg)
        {
			int rs = (shift >> 4) & 0xF;
			// Offset is stored in the lower byte
			offset = regs[rs] & 0xFF;

			// Prefetching
			if (rm == 15)
				value += 4;
			if (rn == 15)
				op1 += 4;
        }
        else
        {
			// Offset is a 5-bit immediate value
			offset = (shift >> 3) & 0x1F;
        }

        int type = (shift >> 1) & 0x3;
        switch (type)
        {
        case 0b00: op2 = lsl(value, offset, carry); break;
        case 0b01: op2 = lsr(value, offset, carry, !use_reg); break;
        case 0b10: op2 = asr(value, offset, carry, !use_reg); break;
        case 0b11: op2 = ror(value, offset, carry, !use_reg); break;
        }

        // Shifting a register adds an I cycle
        cycle();
    }

    if (rd == 15)
    {
        // 1N cycle because writing to PC
        cycle(regs.pc, true);

        if (flags)
        {
            // Change mode and copy SPSR
            u32 spsr = regs.spsr;
            regs.switchMode(static_cast<Mode>(spsr & 0x1F));
            regs.cpsr = spsr;

            flags = false;
        }
    }

    switch (opcode)
    {
    // AND
    case 0b0000:
        dst = op1 & op2;
        if (flags) 
            logical(dst, carry);
        break;

    // EOR
    case 0b0001:
        dst = op1 ^ op2;
        if (flags) 
            logical(dst, carry);
        break;

    // SUB
    case 0b0010:
        dst = op1 - op2;
        if (flags) 
            arithmetic(op1, op2, false);
        break;

    // RSB
    case 0b0011:
        dst = op2 - op1;
        if (flags) 
            arithmetic(op2, op1, false);
        break;

    // ADD
    case 0b0100:
        dst = op1 + op2;
        if (flags) 
            arithmetic(op1, op2, true);
        break;

    // ADC
    case 0b0101:
        op2 += regs.c();
        dst = op1 + op2;
        if (flags) 
            arithmetic(op1, op2, true);
        break;

    // SBC
    case 0b0110:
        op2 += 1 - regs.c();
        dst = op1 - op2;
        if (flags) 
            arithmetic(op1, op2, false);
        break;

    // RBC
    case 0b0111:
        op1 += 1 - regs.c();
        dst = op2 - op1;
        if (flags) 
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
        if (flags) 
            logical(dst, carry);
        break;

    // MOV
    case 0b1101:
        dst = op2;
        if (flags) 
            logical(dst, carry);
        break;

    // BIC
    case 0b1110:
        dst = op1 & ~op2;
        if (flags) 
            logical(dst, carry);
        break;

    // MVN
    case 0b1111:
        dst = ~op2;
        if (flags) 
            logical(dst, carry);
        break;
    }

    if (rd == 15)
    {
        dst = alignWord(dst);
        needs_flush = true;

		// 1S cycle because writing to PC
		cycle(regs.pc, false);
    }
	// 1S cycle
    cycle(regs.pc + 4, false);
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
            if (regs.thumb())
            {
                regs.pc = alignHalf(regs.pc);
                needs_flush = true;
            }
        }
    }
    else  // MRS
    {
        int rd = (instr >> 12) & 0xF;
        regs[rd] = use_spsr ? regs.spsr : regs.cpsr;
    }

	// 1S cycle
	cycle(regs.pc, false);
}

// ARM 5
void ARM::multiply(u32 instruction)
{
    int accumulate = (instruction & 0x00200000);
    int set_flags  = (instruction & 0x00100000);
	int reg_dst	   = (instruction & 0x000F0000) >> 16;
	int reg_acc	   = (instruction & 0x0000F000) >> 12;
	int reg_op1	   = (instruction & 0x00000F00) >> 8;
	int reg_op2	   = (instruction & 0x0000000F) >> 0;

    u32& dst = regs[reg_dst];
    u32 op1 = regs[reg_op1];
    u32 op2 = regs[reg_op2];
    u32 acc = regs[reg_acc];

    dst = op1 * op2;
    if (accumulate) 
        dst += acc;

    if (set_flags)
        logical(dst);

	// Calculate internal cycles
	static u32 masks[3] =
	{
		0xFF000000,
		0xFFFF0000,
		0xFFFFFF00
	};

	int internal = 4;
	for (int x = 0; x < 3; ++x)
	{
		int bits = op1 & masks[x];
		if (bits == 0 || bits == masks[x])
			internal--;
		else
			break;
	}

	for (int x = 0; x < internal; ++x)
		cycle();

	// 1S cycle
	cycle(regs.pc, false);
}

// ARM 6
void ARM::multiplyLong(u32 instr)
{
    bool sign = (instr >> 22) & 0x1;
    bool accumulate = (instr >> 21) & 0x1;
    bool set_flags = (instr >> 20) & 0x1;

    int rdhi = (instr >> 16) & 0xF;
    int rdlo = (instr >> 12) & 0xF;
    int rs = (instr >> 8) & 0xF;
    int rm = instr & 0xF;

    u32& dsthi = regs[rdhi];
    u32& dstlo = regs[rdlo];

    u64 op1 = regs[rm];
    u64 op2 = regs[rs];

    if (sign)
    {
        // Sign extend to 64 bits if negative two's complement
        if (op1 & (1 << 31))
            op1 |= 0xFFFFFFFF00000000;
        if (op2 & (1 << 31))
            op2 |= 0xFFFFFFFF00000000;
    }
    
    u64 result = op1 * op2;

    if (accumulate)
        result += (static_cast<u64>(dsthi) << 32) | dstlo;

    if (set_flags)
    {
        regs.setZ(result == 0);
        regs.setN(result >> 63);
    }

    dsthi = result >> 32;
    dstlo = result & 0xFFFFFFFF;
}

// ARM 7
void ARM::singleTransfer(u32 instr)
{
    bool use_reg = (instr >> 25) & 0x1;
    bool pre_indexing = (instr >> 24) & 0x1;
    bool increment = (instr >> 23) & 0x1;
    int byte = (instr >> 22) & 0x1;
    bool writeback = (instr >> 21) & 0x1;
    int load = (instr >> 20) & 0x1;

    int rn = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;

    u32 offset = instr & 0xFFF;
    if (use_reg)
    {
        bool carry;
        offset = shiftedRegister(offset, carry);
        // Do not use shifted register here, not increase if PC is used as shifted register
    }

    u32 addr = regs[rn];
    u32& dst = regs[rd];

    // Post-indexing always writes back
    writeback |= !pre_indexing;

    if (pre_indexing)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    switch (load << 1 | byte)
    {
    case 0b00:
        // STR
        mmu.writeWord(alignWord(addr), dst);
        break;

    case 0b01:
        // STRB
        mmu.writeByte(addr, dst & 0xFF);
        break;

    case 0b10:
        // LDR
        dst = ldr(addr);
        break;

    case 0b11:
        // LDRB
        dst = mmu.readByte(addr);
        break;
    }

    if (!pre_indexing)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    if (writeback)
    {
        if (rd == rn)
            regs[rn] = dst;
        else
            regs[rn] = addr;
    }
}

// ARM 8
void ARM::halfSignedTransfer(u32 instr)
{
    bool pre_indexing = (instr >> 24) & 0x1;
    bool use_imm = (instr >> 22) & 0x1;
    bool increment = (instr >> 23) & 0x1;
    bool writeback = (instr >> 21) & 0x1;
    bool load = (instr >> 20) & 0x1;

    int rn = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;
    int sign = (instr >> 6) & 0x1;
    int half = (instr >> 5) & 0x1;

    u32 offset;
    if (use_imm)
    {
        int upper = (instr >> 8) & 0xF;
        int lower = instr & 0xF;
        offset = upper << 4 | lower;
    }
    else
    {
        int rm = instr & 0xF;
        offset = regs[rm];
    }

    u32 addr = regs[rn];
    u32& dst = regs[rd];

    // Post-indexing always writes back
    writeback |= !pre_indexing;

    if (pre_indexing)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    switch (sign << 1 | half)
    {
    // SWP
    case 0b00:
        break;

    // STRH / LDRH
    case 0b01:
        if (load)
            dst = ldrh(addr);
        else
            mmu.writeHalf(alignHalf(addr), dst & 0xFFFF);
        break;

    // LDRSB
    case 0b10:
        dst = mmu.readByte(addr);
        if (dst & (1 << 7))
            dst |= 0xFFFFFF00;
        break;

    // LDRSH
    case 0b11:
        dst = ldrsh(addr);
        break;
    }

    if (!pre_indexing)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    if (writeback)
    {
        if (rn == rd)
            regs[rn] = dst;
        else
            regs[rn] = addr;
    }
}

// ARM 9
void ARM::blockTransfer(u32 instr)
{
    bool full = (instr >> 24) & 0x1;
    bool ascending = (instr >> 23) & 0x1;
    bool user_transfer = (instr >> 22) & 0x1;
    bool writeback = (instr >> 21) & 0x1;
    bool load = (instr >> 20) & 0x1;
    int rn = (instr >> 16) & 0xF;
    int rlist = instr & 0xFFFF;

    u32 addr = regs[rn];

    // Handle user transfer
    Mode mode = regs.mode();
    if (user_transfer)
        regs.switchMode(MODE_USR);

    if (rlist != 0)
    {
        // Lowest register gets stored at the lowest address
        if (ascending)
        {
            // Start at lowest address, load / store registers in order
            for (int x = 0; x < 16; ++x)
            {
                if (rlist & 1 << x)
                {
                    if (full)
                        addr += 4;

                    if (load)
                    {
                        regs[x] = mmu.readWord(alignWord(addr));

                        if (x == 15)
                            needs_flush = true;
                    }
                    else
                    {
                        mmu.writeWord(alignWord(addr), regs[x]);
                    }
            
                    if (!full)
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
                    if (full)
                        addr -= 4;

                    if (load)
                    {
                        regs[x] = mmu.readWord(alignWord(addr));

                        if (x == 15)
                            needs_flush = true;
                    }
                    else
                    {
                        mmu.writeWord(alignWord(addr), regs[x]);
                    }

                    if (!full)
                        addr -= 4;
                }
            }
        }
    }
    else  // Special case with empty rlist
    {
        if (load)
        {
            regs.pc = mmu.readWord(alignWord(addr));
            regs.pc = alignWord(regs.pc);

            needs_flush = true;
        }
        else
        {
            mmu.writeWord(alignWord(addr), regs.pc);
        }

        if (full)
            addr += 0x40;
        else
            addr -= 0x40;
    }

    if (writeback)
        regs[rn] = addr;

    if (user_transfer)
        regs.switchMode(mode);
}

// ARM 10
void ARM::singleSwap(u32 instr)
{
    bool byte = (instr >> 22) & 0x1;

    int rb = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;
    int rs = instr & 0xF;

    u32 addr = regs[rb];
    u32& dst = regs[rd];
    u32 src = regs[rs];

    if (byte)
    {
        dst = mmu.readByte(addr);
        mmu.writeByte(addr, src & 0xFF);
    }
    else
    {
        dst = ldr(addr);
        mmu.writeWord(alignWord(addr), src);
    }
}

// ARM 11
void ARM::swiArm(u32 instr)
{
    std::cout << "Unimplemented ARM SWI\n";
}
