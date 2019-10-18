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

void ARM::HWI()
{
    // Returns with subs pc, lr, 4
    u32 lr = pc - 2 * (cpsr.thumb ? 2 : 4) + 4;

    interrupt(0x18, lr, PSR::Mode::IRQ);
}

void ARM::SWI()
{
    // Returns with movs pc, lr
    u32 lr = pc - (cpsr.thumb ? 2 : 4);

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

void ARM::execute()
{
    if (cpsr.thumb)
    {
        u16 instr = readHalf(pc - 4);

        (this->*lut_thumb[instr >> 6])(instr);
    }
    else
    {
        u32 instr = readWord(pc - 8);

        if (cpsr.check(PSR::Condition(instr >> 28)))
        {
            switch (decodeArm(instr))
            {
            case InstructionArm::Undefined: break;
            case InstructionArm::BranchExchange: Arm_BranchExchange(instr); break;
            case InstructionArm::BranchLink: Arm_BranchLink(instr); break;
            case InstructionArm::DataProcessing: Arm_DataProcessing(instr); break;
            case InstructionArm::PSRTransfer: Arm_StatusTransfer(instr); break;
            case InstructionArm::Multiply: Arm_Multiply(instr); break;
            case InstructionArm::MultiplyLong: Arm_MultiplyLong(instr); break;
            case InstructionArm::SingleDataTransfer: Arm_SingleDataTransfer(instr); break;
            case InstructionArm::HalfwordSignedDataTransfer: Arm_HalfSignedDataTransfer(instr); break;
            case InstructionArm::BlockDataTransfer: Arm_BlockDataTransfer(instr); break;
            case InstructionArm::SingleDataSwap: Arm_SingleDataSwap(instr); break;
            case InstructionArm::SoftwareInterrupt: Arm_SoftwareInterrupt(instr); break;

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
    advance();
}

void ARM::debug()
{
    u32 instr = cpsr.thumb
        ? readHalf(pc - 4)
        : readWord(pc - 8);

    fmt::printf("%08X  %08X  %08X  %s\n", 
        cycles, 
        pc - 2 * (cpsr.thumb ? 2 : 4), 
        instr, 
        Disassembler::disassemble(instr, *this)
    );
}
