#include "arm.h"

#include <fmt/printf.h>

#include "common/integer.h"
#include "common/macros.h"
#include "common/utility.h"
//#include "mmu/interrupt.h"
#include "disassembler.h"
#include "decode.h"

void ARM::reset()
{
    Registers::reset();

    cycles = 0;
}

int ARM::emulate()
{
    u64 last = cycles;

    debug();

    //if (Interrupt::requested() && !cpsr.irqd)
    //    hardwareInterrupt();
    //else
        execute();

    advance();

    return static_cast<int>(cycles - last);
}

u32 ARM::logical(u32 result, bool flags)
{
    if (flags)
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 31;
    }
    return result;
}

u32 ARM::logical(u32 result, bool carry, bool flags)
{
    if (flags)
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 31;
        cpsr.c = carry;
    }
    return result;
}

u32 ARM::add(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 + op2;

    if (flags)
    {
        int msb_op1 = op1 >> 31;
        int msb_op2 = op2 >> 31;

        cpsr.z = result == 0;
        cpsr.n = result >> 31;
        cpsr.c = op2 > (0xFFFFFFFF - op1);
        cpsr.v = msb_op1 == msb_op2 && (result >> 31) != msb_op1;
    }
    return result;
}

u32 ARM::sub(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 - op2;

    if (flags)
    {
        int msb_op1 = op1 >> 31;
        int msb_op2 = op2 >> 31;

        cpsr.z = result == 0;
        cpsr.n = result >> 31;
        cpsr.c = op2 <= op1;
        cpsr.v = msb_op1 != msb_op2 && (result >> 31) == msb_op2;
    }
    return result;
}

void ARM::hardwareInterrupt()
{
    // Returns with subs pc, lr, 4
    u32 lr = pc - 2 * length() + 4;

    interrupt(0x18, lr, PSR::Mode::IRQ);
}

void ARM::softwareInterrupt()
{
    // Returns with movs pc, lr
    u32 lr = pc - length();

    interrupt(0x08, lr, PSR::Mode::SVC);
}

void ARM::interrupt(u32 pc, u32 lr, PSR::Mode mode)
{
    cycle<Access::Nonseq>(this->pc);

    u32 cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->lr = lr;
    this->pc = pc;

    this->cpsr.thumb = false;
    this->cpsr.irqd  = true;

    refill<State::Arm>();
}

int ARM::length() const
{
    static constexpr int lengths[2] = { 4, 2 };

    return lengths[cpsr.thumb];
}

void ARM::execute()
{
    if (cpsr.thumb)
    {
        u16 instr = readHalf(pc - 4);

        switch (decodeThumb(instr))
        {
        case InstructionThumb::Undefined: break;
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
        case InstructionThumb::SoftwareInterrupt: softwareInterrupt(); break;
        case InstructionThumb::UnconditionalBranch: unconditionalBranch(instr); break;
        case InstructionThumb::LongBranchLink: longBranchLink(instr); break;

        default:
            EGG_UNREACHABLE;
            break;
        }
    }
    else
    {
        u32 instr = readWord(pc - 8);

        if (cpsr.check(static_cast<PSR::Condition>(instr >> 28)))
        {
            switch (decodeArm(instr))
            {
            case InstructionArm::Undefined: break;
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
            case InstructionArm::SoftwareInterrupt: softwareInterrupt(); break;

            default:
                EGG_UNREACHABLE;
                break;
            }
        }
        else
        {
            cycle<Access::Seq>(pc + 8);
        }
    }
}

void ARM::debug()
{
    u32 instr = cpsr.thumb
        ? readHalf(pc - 4)
        : readWord(pc - 8);

    fmt::printf("%08X  %08X  %08X  %s\n", 
        cycles, 
        pc - 2 * length(), 
        instr, 
        Disassembler::disassemble(instr, *this)
    );
}

void ARM::logical_old(u32 result)
{
    cpsr.z = result == 0;
    cpsr.n = result >> 31;
}

void ARM::logical_old(u32 result, bool carry)
{
    cpsr.z = result == 0;
    cpsr.n = result >> 31;
    cpsr.c = carry;
}

void ARM::arithmetic(u32 op1, u32 op2, bool addition)
{
    u32 result = addition
        ? op1 + op2
        : op1 - op2;

    cpsr.z = result == 0;
    cpsr.n = result >> 31;

    cpsr.c = addition
        ? op2 > (0xFFFFFFFF - op1)
        : op2 <= op1;

    int msb_op1 = op1 >> 31;
    int msb_op2 = op2 >> 31;

    cpsr.v = addition
        ? msb_op1 == msb_op2 && (result >> 31) != msb_op1
        : msb_op1 != msb_op2 && (result >> 31) == msb_op2;
}

void ARM::cycle(u32 addr, AccessType access)
{
    cycles++;

    static constexpr int seq[3][2] = { { 1, 2 }, { 1, 4 }, { 1, 8 } };
    static constexpr int nonseq[4] = { 4, 3, 2, 8 };

    //switch (addr >> 24)
    //{
    //case PAGE_PALETTE:
    //case PAGE_VRAM:
    //case PAGE_OAM:
    //    if (!mmio.dispstat.hblank && !mmio.dispstat.vblank)
    //        cycles++;
    //    break;

    //case PAGE_GAMEPAK_0:
    //case PAGE_GAMEPAK_0+1:
    //    if (access == SEQ)
    //        cycles += seq[0][mmio.waitcnt.ws0.s];
    //    else
    //        cycles += nonseq[mmio.waitcnt.ws0.n];
    //    break;

    //case PAGE_GAMEPAK_1:
    //case PAGE_GAMEPAK_1+1:
    //    if (access == SEQ)
    //        cycles += seq[1][mmio.waitcnt.ws1.s];
    //    else
    //        cycles += nonseq[mmio.waitcnt.ws1.n];
    //    break;

    //case PAGE_GAMEPAK_2:
    //case PAGE_GAMEPAK_2+1:
    //    if (access == SEQ)
    //        cycles += seq[2][mmio.waitcnt.ws2.s];
    //    else
    //        cycles += nonseq[mmio.waitcnt.ws2.n];
    //    break;

    //default:
    //    if (addr >= MAP_GAMEPAK_SRAM)
    //        cycles += nonseq[mmio.waitcnt.sram];
    //    break;
    //}
}
