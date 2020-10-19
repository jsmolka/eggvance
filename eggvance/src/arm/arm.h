#pragma once

#include <array>

#include "arm/io.h"
#include "arm/registers.h"

enum Irq
{
    kIrqVBlank  = 1 << 0,
    kIrqHBlank  = 1 << 1,
    kIrqVMatch  = 1 << 2,
    kIrqTimer0  = 1 << 3,
    kIrqTimer1  = 1 << 4,
    kIrqTimer2  = 1 << 5,
    kIrqTimer3  = 1 << 6,
    kIrqSerial  = 1 << 7,
    kIrqDma0    = 1 << 8,
    kIrqDma1    = 1 << 9,
    kIrqDma2    = 1 << 10,
    kIrqDma3    = 1 << 11,
    kIrqGamepad = 1 << 12,
    kIrqGamePak = 1 << 13
};

enum
{
    kStateThumb = 1 << 0,
    kStateHalt  = 1 << 1,
    kStateIrq   = 1 << 2,
    kStateDma   = 1 << 3,
    kStateTimer = 1 << 4
};

class Arm : public Registers
{
public:
    friend class Io;
    friend class DmaChannel;
    friend class Mmu;

    Arm();

    void run(int cycles);
    void raise(uint irq);

    uint state = 0;

private:
    using Instruction32 = void(Arm::*)(u32);
    using Instruction16 = void(Arm::*)(u16);

    enum Shift
    {
        kShiftLsl,
        kShiftLsr,
        kShiftAsr,
        kShiftRor
    };

    template<bool Immediate> u32 lsl(u32 value, u32 amount, bool flags = true);
    template<bool Immediate> u32 lsr(u32 value, u32 amount, bool flags = true);
    template<bool Immediate> u32 asr(u32 value, u32 amount, bool flags = true);
    template<bool Immediate> u32 ror(u32 value, u32 amount, bool flags = true);

    template<typename Integral>
    Integral log(Integral value, bool flags = true);

    u32 add(u32 op1, u32 op2, bool flags = true);
    u32 sub(u32 op1, u32 op2, bool flags = true);
    u32 adc(u32 op1, u32 op2, bool flags = true);
    u32 sbc(u32 op1, u32 op2, bool flags = true);

    bool isSequential(u32 addr) const;

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u32 readWordRotate(u32 addr);
    u32 readHalfRotate(u32 addr);
    u32 readHalfSignEx(u32 addr);

    void flushHalf();
    void flushWord();

    template<uint State>
    void dispatch();

    void idle();

    template<bool Signed>
    void booth(u32 multiplier);

    void interrupt(u32 pc, u32 lr, PSR::Mode mode);
    void interruptHw();
    void interruptSw();
    void processIrq();

    template<uint Instr> void Arm_BranchExchange(u32 instr);
    template<uint Instr> void Arm_BranchLink(u32 instr);
    template<uint Instr> void Arm_DataProcessing(u32 instr);
    template<uint Instr> void Arm_StatusTransfer(u32 instr);
    template<uint Instr> void Arm_Multiply(u32 instr);
    template<uint Instr> void Arm_MultiplyLong(u32 instr);
    template<uint Instr> void Arm_SingleDataTransfer(u32 instr);
    template<uint Instr> void Arm_HalfSignedDataTransfer(u32 instr);
    template<uint Instr> void Arm_BlockDataTransfer(u32 instr);
    template<uint Instr> void Arm_SingleDataSwap(u32 instr);
    template<uint Instr> void Arm_SoftwareInterrupt(u32 instr);
    template<uint Instr> void Arm_CoprocessorDataOperations(u32 instr);
    template<uint Instr> void Arm_CoprocessorDataTransfers(u32 instr);
    template<uint Instr> void Arm_CoprocessorRegisterTransfers(u32 instr);
    template<uint Instr> void Arm_Undefined(u32 instr);

    template<uint Instr> void Thumb_MoveShiftedRegister(u16 instr);
    template<uint Instr> void Thumb_AddSubtract(u16 instr);
    template<uint Instr> void Thumb_ImmediateOperations(u16 instr);
    template<uint Instr> void Thumb_AluOperations(u16 instr);
    template<uint Instr> void Thumb_HighRegisterOperations(u16 instr);
    template<uint Instr> void Thumb_LoadPcRelative(u16 instr);
    template<uint Instr> void Thumb_LoadStoreRegisterOffset(u16 instr);
    template<uint Instr> void Thumb_LoadStoreByteHalf(u16 instr);
    template<uint Instr> void Thumb_LoadStoreImmediateOffset(u16 instr);
    template<uint Instr> void Thumb_LoadStoreHalf(u16 instr);
    template<uint Instr> void Thumb_LoadStoreSpRelative(u16 instr);
    template<uint Instr> void Thumb_LoadRelativeAddress(u16 instr);
    template<uint Instr> void Thumb_AddOffsetSp(u16 instr);
    template<uint Instr> void Thumb_PushPopRegisters(u16 instr);
    template<uint Instr> void Thumb_LoadStoreMultiple(u16 instr);
    template<uint Instr> void Thumb_ConditionalBranch(u16 instr);
    template<uint Instr> void Thumb_SoftwareInterrupt(u16 instr);
    template<uint Instr> void Thumb_UnconditionalBranch(u16 instr);
    template<uint Instr> void Thumb_LongBranchLink(u16 instr);
    template<uint Instr> void Thumb_Undefined(u16 instr);

    template<uint Hash> static constexpr Instruction32 Arm_Decode();
    template<uint Hash> static constexpr Instruction16 Thumb_Decode();

    static const std::array<Instruction32, 4096> instr_arm;
    static const std::array<Instruction16, 1024> instr_thumb;

    int cycles    = 0;
    u32 prev_addr = 0;
    u32 pipe[2]   = {};

    HaltControl haltcnt;
    WaitControl waitcnt;

    struct
    {
        IrqMaster  master;
        IrqEnable  enable;
        IrqRequest request;
    } irq;
};

inline Arm arm;

#include "arm.inl"
