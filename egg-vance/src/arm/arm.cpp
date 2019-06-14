#include "arm.h"

#include <array>

#include "common/format.h"
#include "mmu/map.h"
#include "enums.h"
#include "disassembler.h"
#include "utility.h"

ARM::ARM(MMU& mmu)
    : mmu(mmu)
{

}

void ARM::reset()
{
    regs.reset();

    flush();
}

int ARM::step()
{
    cycles = 0;

    fetch(pipe[0]);
    decode(pipe[1]);

    #ifdef _DEBUG
    debug(pipe[2]);
    #endif

    execute(pipe[2]);

    if (needs_flush)
        flush();
    else
        advance();

    return cycles;
}

void ARM::fetch(ARM::PipeState& state)
{
    if (regs.thumb)
    {
        state.data = mmu.readHalf(regs.pc);
        state.thumb = ThumbInstr::Invalid;
    }
    else
    {
        state.data = mmu.readWord(regs.pc);
        state.arm = ArmInstr::Invalid;
    }
    state.refill = false;
}

void ARM::decode(ARM::PipeState& state)
{
    if (state.refill)
        return;

    if (regs.thumb)
        state.thumb = decoder::decodeThumb(static_cast<u16>(state.data));
    else
        state.arm = decoder::decodeArm(state.data);
}

void ARM::execute(ARM::PipeState& state)
{
    if (state.refill)
        return;

    if (regs.thumb)
    {
        u16 instr = static_cast<u16>(state.data);

        switch (state.thumb)
        {
        case ThumbInstr::MoveShiftedRegister: moveShiftedRegister(instr); break;
        case ThumbInstr::AddSubImmediate: addSubImmediate(instr); break;
        case ThumbInstr::AddSubMovCmpImmediate: addSubMovCmpImmediate(instr); break;
        case ThumbInstr::AluOperations: aluOperations(instr); break;
        case ThumbInstr::HighRegisterBranchExchange: highRegisterBranchExchange(instr); break;
        case ThumbInstr::LoadPcRelative: loadPcRelative(instr); break;
        case ThumbInstr::LoadStoreRegisterOffset: loadStoreRegisterOffset(instr); break;
        case ThumbInstr::LoadStoreHalfSigned: loadStoreHalfSigned(instr); break;
        case ThumbInstr::LoadStoreImmediateOffset: loadStoreImmediateOffset(instr); break;
        case ThumbInstr::LoadStoreHalf: loadStoreHalf(instr); break;
        case ThumbInstr::LoadStoreSpRelative: loadStoreSpRelative(instr); break;
        case ThumbInstr::LoadAddress: loadAddress(instr); break;
        case ThumbInstr::AddOffsetSp: addOffsetSp(instr); break;
        case ThumbInstr::PushPopRegisters: pushPopRegisters(instr); break;
        case ThumbInstr::LoadStoreMultiple: loadStoreMultiple(instr); break;
        case ThumbInstr::ConditionalBranch: conditionalBranch(instr); break;
        case ThumbInstr::SWI: swiThumb(instr); break;
        case ThumbInstr::UnconditionalBranch: unconditionalBranch(instr); break;
        case ThumbInstr::LongBranchLink: longBranchLink(instr); break;
        }
    }
    else
    {
        u32 instr = state.data;

        if (regs.check(static_cast<Condition>(instr >> 28)))
        {
            switch (state.arm)
            {
            case ArmInstr::BranchExchange: branchExchange(instr); break;
            case ArmInstr::BranchLink: branchLink(instr); break;
            case ArmInstr::DataProcessing: dataProcessing(instr); break;
            case ArmInstr::PsrTransfer: psrTransfer(instr); break;
            case ArmInstr::Multiply: multiply(instr); break;
            case ArmInstr::MultiplyLong: multiplyLong(instr); break;
            case ArmInstr::SingleTransfer: singleTransfer(instr); break;
            case ArmInstr::HalfSignedTransfer: halfSignedTransfer(instr); break;
            case ArmInstr::BlockTransfer: blockTransfer(instr); break;
            case ArmInstr::SingleSwap: singleSwap(instr); break;
            case ArmInstr::SWI: swiArm(instr); break;
            case ArmInstr::CoDataOperation:
            case ArmInstr::CoDataTransfer:
            case ArmInstr::CoRegisterTransfer: fmt::printf("Coprocessor instruction %08X\n", instr); break;
            case ArmInstr::Invalid: fmt::printf("Invalid instruction %08X\n", instr); break;
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
    pipe[2] = pipe[1];
    pipe[1] = pipe[0];

    regs.pc += regs.thumb ? 2 : 4;
}


void ARM::debug(ARM::PipeState& state)
{
    if (pipe[2].refill)
        return;
    
    fmt::printf("%08X  %08X  %s\n",
        regs.pc - (regs.thumb ? 4 : 8),
        state.data,
        Disassembler::disassemble(state.data, regs)
    );
}

void ARM::flush()
{
    pipe[0].refill = true;
    pipe[1].refill = true;
    pipe[2].refill = true;

    needs_flush = false;
}

void ARM::updateZ(u32 result)
{
    regs.z = result == 0;
}

void ARM::updateN(u32 result)
{
    regs.n = result >> 31;
}

void ARM::updateC(u32 op1, u32 op2, bool addition)
{
    bool carry;

    if (addition)
        carry = op2 > (0xFFFFFFFF - op1);
    else
        carry = op2 <= op1;

    regs.c = carry;
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

    regs.v = overflow;
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

    regs.c = carry;
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
    static const std::array<u32, 3> masks = 
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
