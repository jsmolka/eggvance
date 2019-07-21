#include "arm.h"

#include "common/format.h"
#include "mmu/map.h"
#include "enums.h"
#include "disassembler.h"
#include "utility.h"

static u64 total_cycles = 0;

ARM::ARM(MMU& mmu)
    : mmu(mmu)
{

}

void ARM::reset()
{
    regs.reset();
}

void ARM::interrupt()
{
    // Check if interrupts are disabled
    if (regs.irq_disable)
        return;

    u32 cpsr = regs.cpsr;
    u32 next;

    next = regs.pc - (regs.thumb ? 4 : 8);

    regs.switchMode(MODE_IRQ);
    regs.spsr = cpsr;
    // Interrupts return with subs pc, lr, 4
    regs.lr = next + 4;

    regs.thumb = false;
    regs.irq_disable = true;

    regs.pc = EXV_IRQ;
    
    advance();
    advance();
}

int ARM::step()
{
    cycles = 0;
     
    //if (total_cycles > 0x00E68312)
    //    debug();

    execute();
    advance();

    total_cycles += cycles;

    return cycles;
}

void ARM::execute()
{
    if (regs.thumb)
    {
        u16 instr = mmu.readHalf(regs.pc - 4);

        switch (decodeThumb(instr))
        {
        case InstructionThumb::MoveShiftedRegister: moveShiftedRegister(instr); break;
        case InstructionThumb::AddSubtractImmediate: addSubtractImmediate(instr); break;
        case InstructionThumb::AddSubtractMoveCompareImmediate: addSubtractMoveCompareImmediate(instr); break;
        case InstructionThumb::ALUOperations: aluOperations(instr); break;
        case InstructionThumb::HighRegisterBranchExchange: highRegisterBranchExchange(instr); break;
        case InstructionThumb::LoadPCRelative: loadPCRelative(instr); break;
        case InstructionThumb::LoadStoreRegisterOffset: loadStoreRegisterOffset(instr); break;
        case InstructionThumb::LoadStoreHalfwordSigned: loadStoreHalfwordSigned(instr); break;
        case InstructionThumb::LoadStoreImmediateOffset: loadStoreImmediateOffset(instr); break;
        case InstructionThumb::LoadStoreHalfword: loadStoreHalfword(instr); break;
        case InstructionThumb::LoadStoreSPRelative: loadStoreSPRelative(instr); break;
        case InstructionThumb::LoadAddress: loadAddress(instr); break;
        case InstructionThumb::AddOffsetSP: addOffsetSP(instr); break;
        case InstructionThumb::PushPopRegisters: pushPopRegisters(instr); break;
        case InstructionThumb::LoadStoreMultiple: loadStoreMultiple(instr); break;
        case InstructionThumb::ConditionalBranch: conditionalBranch(instr); break;
        case InstructionThumb::SoftwareInterrupt: softwareInterruptThumb(instr); break;
        case InstructionThumb::UnconditionalBranch: unconditionalBranch(instr); break;
        case InstructionThumb::LongBranchLink: longBranchLink(instr); break;
        }
    }
    else
    {
        u32 instr = mmu.readWord(regs.pc - 8);

        if (regs.check(static_cast<Condition>(instr >> 28)))
        {
            switch (decodeArm(instr))
            {
            case InstructionArm::BranchExchange: branchExchange(instr); break;
            case InstructionArm::BranchLink: branchLink(instr); break;
            case InstructionArm::DataProcessing: dataProcessing(instr); break;
            case InstructionArm::PSRTransfer: psrTransfer(instr); break;
            case InstructionArm::Multiply: multiply(instr); break;
            case InstructionArm::MultiplyLong: multiplyLong(instr); break;
            case InstructionArm::SingleDataTransfer: singleDataTransfer(instr); break;
            case InstructionArm::HalfwordSignedDataTransfer: halfwordSignedDataTransfer(instr); break;
            case InstructionArm::BlockDataTransfer: blockDataTransfer(instr); break;
            case InstructionArm::SingleDataSwap: singleDataSwap(instr); break;
            case InstructionArm::SoftwareInterrupt: softwareInterruptArm(instr); break;
            }
        }
        else
        {
            cycle(regs.pc + 4, SEQ);
        }
    }
}

void ARM::advance()
{
    regs.pc += (regs.thumb ? 2 : 4);
}

void ARM::debug()
{
    u32 pc = regs.thumb
        ? regs.pc - 4
        : regs.pc - 8;

    u32 data = regs.thumb
        ? mmu.readHalf(pc)
        : mmu.readWord(pc);

    fmt::printf("%08X  %08X  %08X  %s\n",
        total_cycles,
        pc,
        data,
        Disassembler::disassemble(data, regs)
    );
}

void ARM::logical(u32 result)
{
    regs.z = result == 0;
    regs.n = result >> 31;
}

void ARM::logical(u32 result, bool carry)
{
    regs.z = result == 0;
    regs.n = result >> 31;
    regs.c = carry;
}

void ARM::arithmetic(u32 op1, u32 op2, bool addition)
{
    u32 result = addition
        ? op1 + op2
        : op1 - op2;

    regs.z = result == 0;
    regs.n = result >> 31;

    regs.c = addition
        ? op2 > (0xFFFFFFFF - op1)
        : op2 <= op1;

    int msb_op1 = op1 >> 31;
    int msb_op2 = op2 >> 31;

    regs.v = addition
        ? msb_op1 == msb_op2 && (result >> 31) != msb_op1
        : msb_op1 != msb_op2 && (result >> 31) == msb_op2;
}

u32 ARM::lsl(u32 value, int offset, bool& carry)
{
    if (offset != 0)
    {
        if (offset < 32)
        {
            carry = (value >> (32 - offset)) & 0x1;
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
        carry = regs.c;
    }
    return value;
}

u32 ARM::lsr(u32 value, int offset, bool& carry, bool immediate)
{
    if (offset != 0 && offset != 32)
    {
        if (offset < 32)
        {
            carry = (value >> (offset - 1)) & 0x1;
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
        if (immediate)
        {
            carry = value >> 31;
            value = 0;
        }
        else
        {
            carry = regs.c;
        }
    }
    return value;
}

// Todo: convert to s32 and use shift operator?
u32 ARM::asr(u32 value, int offset, bool& carry, bool immediate)
{
    if (offset != 0)
    {
        if (offset < 32)
        {
            bool msb = value >> 31;

            carry = value >> (offset - 1) & 0x1;
            value >>= offset;

            if (msb)
                value |= 0xFFFFFFFF << (31 - offset);
        }
        else
        {
            carry = value >> 31;
            value = carry ? 0xFFFFFFFF : 0;
        }
    }
    else  // Special case ASR #32 (assembles to ASR #0)
    {
        if (immediate)
        {
            carry = value >> 31;
            value = carry ? 0xFFFFFFFF : 0;
        }
        else
        {
            carry = regs.c;
        }
    }
    return value;
}

u32 ARM::ror(u32 value, int offset, bool& carry, bool immediate)
{
    if (offset != 0)
    {
        offset %= 32;

        if (offset != 0)
            value = (value << (32 - offset)) | (value >> offset);

        carry = value >> 31;
    }
    else  // Special case ROR #0 (RRX)
    {
        if (immediate)
        {
            carry = value & 0x1;
            value >>= 1;
            value |= regs.c << 31;
        }
        else
        {
            carry = regs.c;
        }
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
        value = mmu.readWord(alignWord(addr));
        value = ror(value, rotation, carry);
    }
    else
    {
        value = mmu.readWord(addr);
    }
    return value;
}

u32 ARM::ldrh(u32 addr)
{
    u32 value;
    if (misalignedHalf(addr))
    {
        bool carry;
        value = mmu.readHalf(alignHalf(addr));
        value = ror(value, 8, carry);
    }
    else
    {
        value = mmu.readHalf(addr);
    }
    return value;
}

u32 ARM::ldrsh(u32 addr)
{
    u32 value;
    if (misalignedHalf(addr))
    {
        value = mmu.readByte(addr);
        if (value & (1 << 7))
            value |= 0xFFFFFF00;
    }
    else
    {
        value = mmu.readHalf(addr);
        if (value & (1 << 15))
            value |= 0xFFFF0000;
    }
    return value;
}

void ARM::cycle()
{
    cycles++;
}

void ARM::cycle(u32 addr, MemoryAccess access)
{
    cycles++;

    static const int nonseq_lut[4] = { 4, 3, 2, 8 };

    if (addr >= MAP_PALETTE && addr < (MAP_OAM + 0x400))
    {
        // Add one cycle if accessing VRAM and not in HBlank or VBlank
        if (!mmu.dispstat.hblank || !mmu.dispstat.vblank)
            cycles++;
    }
    else if (addr >= MAP_GAMEPAK_0 && addr < MAP_GAMEPAK_1)
    {
        if (access == NSEQ)
            cycles += nonseq_lut[mmu.waitcnt.nonseq0];
        else
            cycles += mmu.waitcnt.seq0 ? 1 : 2;
    }
    else if (addr >= MAP_GAMEPAK_1 && addr < MAP_GAMEPAK_2)
    {
        if (access == NSEQ)
            cycles += nonseq_lut[mmu.waitcnt.nonseq1];
        else
            cycles += mmu.waitcnt.seq1 ? 1 : 4;
    }
    else if (addr >= MAP_GAMEPAK_2 && addr < MAP_GAMEPAK_SRAM)
    {
        if (access == NSEQ)
            cycles += nonseq_lut[mmu.waitcnt.nonseq2];
        else
            cycles += mmu.waitcnt.seq2 ? 1 : 8;
    }
    else if (addr >= MAP_GAMEPAK_SRAM && addr < (MAP_GAMEPAK_SRAM + 0x10000))
    {
        cycles += nonseq_lut[mmu.waitcnt.sram];
    }
}

void ARM::cycleMultiplication(u32 multiplier, bool allow_ones)
{
    static constexpr int masks[3] = 
    {
        0xFF000000,
        0xFFFF0000,
        0xFFFFFF00
    };

    int internal = 4;
    for (u32 mask : masks)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (allow_ones && bits == mask))
            internal--;
        else
            break;
    }
    cycles += internal;
}
