#include "arm.h"

#include <fmt/printf.h>

#include "common/integer.h"
#include "common/macros.h"
#include "common/utility.h"
#include "mmu/mmu.h"
#include "decode.h"
#include "diasm.h"

ARM arm;

void ARM::reset()
{
    Registers::reset();

    io.irq_master.reset();
    io.irq_enabled.reset();
    io.irq_request.reset();
    io.waitcnt.reset();
    io.haltcnt.reset();

    dma.reset();
    timer.reset();

    cycles = 0;

    flushPipeWord();
    pc += 4;
}

void ARM::run(int cycles_)
{
    cycles += cycles_;

    while (cycles > 0)
    {
        int last = cycles;

        if (dma.active)
        {
            dma.run(cycles);
            timer.run(last - cycles);
        }
        else
        {
            if (io.haltcnt)
            {
                timer.runUntil(cycles);
            }
            else
            {
                execute();
                timer.run(last - cycles);
            }
        }
    }
}

void ARM::request(Interrupt flag)
{
    if (io.irq_enabled & static_cast<int>(flag))
        io.haltcnt = false;

    io.irq_request |= static_cast<int>(flag);
}

void ARM::execute()
{
    //#ifdef EGG_DEBUG
    //disasm();
    //#endif

    if (io.irq_master && (io.irq_enabled & io.irq_request) && !cpsr.irqd)
    {
        interruptHW();
    }
    else
    {            
        if (cpsr.thumb)
        {
            u16 instr = pipe[0];

            pipe[0] = pipe[1];
            pipe[1] = readHalf(pc);

            switch (decodeThumb(instr))
            {
            case InstructionThumb::MoveShiftedRegister: Thumb_MoveShiftedRegister(instr); break;
            case InstructionThumb::AddSubtract: Thumb_AddSubtract(instr); break;
            case InstructionThumb::ImmediateOperations: Thumb_ImmediateOperations(instr); break;
            case InstructionThumb::ALUOperations: Thumb_ALUOperations(instr); break;
            case InstructionThumb::HighRegisterOperations: Thumb_HighRegisterOperations(instr); break;
            case InstructionThumb::LoadPCRelative: Thumb_LoadPCRelative(instr); break;
            case InstructionThumb::LoadStoreRegisterOffset: Thumb_LoadStoreRegisterOffset(instr); break;
            case InstructionThumb::LoadStoreByteHalf: Thumb_LoadStoreByteHalf(instr); break;
            case InstructionThumb::LoadStoreImmediateOffset: Thumb_LoadStoreImmediateOffset(instr); break;
            case InstructionThumb::LoadStoreHalf: Thumb_LoadStoreHalf(instr); break;
            case InstructionThumb::LoadStoreSPRelative: Thumb_LoadStoreSPRelative(instr); break;
            case InstructionThumb::LoadRelativeAddress: Thumb_LoadRelativeAddress(instr); break;
            case InstructionThumb::AddOffsetSP: Thumb_AddOffsetSP(instr); break;
            case InstructionThumb::PushPopRegisters: Thumb_PushPopRegisters(instr); break;
            case InstructionThumb::LoadStoreMultiple: Thumb_LoadStoreMultiple(instr); break;
            case InstructionThumb::ConditionalBranch: Thumb_ConditionalBranch(instr); break;
            case InstructionThumb::SoftwareInterrupt: Thumb_SoftwareInterrupt(instr); break;
            case InstructionThumb::UnconditionalBranch: Thumb_UnconditionalBranch(instr); break;
            case InstructionThumb::LongBranchLink: Thumb_LongBranchLink(instr); break;
            case InstructionThumb::Undefined: Thumb_Undefined(instr); break;

            default:
                EGG_UNREACHABLE;
                break;
            }
        }
        else
        {
            u32 instr = pipe[0];

            pipe[0] = pipe[1];
            pipe[1] = readWord(pc);

            if (cpsr.check(PSR::Condition(instr >> 28)))
            {
                switch (decodeArm(instr))
                {
                case InstructionArm::BranchExchange: Arm_BranchExchange(instr); break;
                case InstructionArm::BranchLink: Arm_BranchLink(instr); break;
                case InstructionArm::DataProcessing: Arm_DataProcessing(instr); break;
                case InstructionArm::StatusTransfer: Arm_StatusTransfer(instr); break;
                case InstructionArm::Multiply: Arm_Multiply(instr); break;
                case InstructionArm::MultiplyLong: Arm_MultiplyLong(instr); break;
                case InstructionArm::SingleDataTransfer: Arm_SingleDataTransfer(instr); break;
                case InstructionArm::HalfSignedDataTransfer: Arm_HalfSignedDataTransfer(instr); break;
                case InstructionArm::BlockDataTransfer: Arm_BlockDataTransfer(instr); break;
                case InstructionArm::SingleDataSwap: Arm_SingleDataSwap(instr); break;
                case InstructionArm::SoftwareInterrupt: Arm_SoftwareInterrupt(instr); break;
                case InstructionArm::CoprocessorDataOperations: Arm_CoprocessorDataOperations(instr); break;
                case InstructionArm::CoprocessorDataTransfers: Arm_CoprocessorDataTransfers(instr); break;
                case InstructionArm::CoprocessorRegisterTransfers: Arm_CoprocessorRegisterTransfers(instr); break;
                case InstructionArm::Undefined: Arm_Undefined(instr); break;

                default:
                    EGG_UNREACHABLE;
                    break;
                }
            }
        }
    }
    pc += cpsr.thumb ? 2 : 4;
}

void ARM::flushPipeHalf()
{
    pipe[0] = readHalf(pc + 0);
    pipe[1] = readHalf(pc + 2);
    pc += 2;
}

void ARM::flushPipeWord()
{
    pipe[0] = readWord(pc + 0);
    pipe[1] = readWord(pc + 4);
    pc += 4;
}

void ARM::idle()
{
    cycles--;
}

void ARM::booth(u32 multiplier, bool ones)
{
    static constexpr u32 masks[3] =
    {
        0xFF00'0000,
        0xFFFF'0000,
        0xFFFF'FF00
    };

    int internal = 4;
    for (u32 mask : masks)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (ones && bits == mask))
            internal--;
        else
            break;
    }
    cycles -= internal;
}

void ARM::interrupt(u32 pc, u32 lr, PSR::Mode mode)
{
    u32 cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->lr = lr;
    this->pc = pc;

    this->cpsr.thumb = false;
    this->cpsr.irqd  = true;

    flushPipeWord();
}

void ARM::interruptHW()
{
    // Returns with subs pc, lr, 4
    u32 lr = pc - 2 * (cpsr.thumb ? 2 : 4) + 4;

    interrupt(0x18, lr, PSR::Mode::IRQ);
}

void ARM::interruptSW()
{
    // Returns with movs pc, lr
    u32 lr = pc - (cpsr.thumb ? 2 : 4);

    interrupt(0x08, lr, PSR::Mode::SVC);
}

void ARM::disasm()
{
    u32 instr = cpsr.thumb
        ? mmu.readHalf(pc - 4)
        : mmu.readWord(pc - 8);

    fmt::printf("%08X  %08X  %08X  %s\n", 
        cycles, 
        pc - 2 * (cpsr.thumb ? 2 : 4), 
        instr, 
        Disassembler::disassemble(instr, *this)
    );
}
