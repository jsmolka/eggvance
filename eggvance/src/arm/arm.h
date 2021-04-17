#pragma once

#include "bios.h"
#include "io.h"
#include "pipeline.h"
#include "registers.h"
#include "scheduler/event.h"

class Arm : public Registers
{
public:
    friend class Dma;
    friend class DmaChannel;
    friend class InterruptEnable;
    friend class InterruptRequest;
    friend class InterruptMaster;

    enum class State
    {
        Thumb = 1 << 0,
        Halt  = 1 << 1,
        Irq   = 1 << 2,
        Dma   = 1 << 3
    };

    Arm();

    void init();
    void run(u64 cycles);
    void raise(Irq irq, u64 late = 0);

    uint state = 0;

private:
    enum class Shift { Lsl, Lsr, Asr, Ror };

    using Instruction32 = void(Arm::*)(u32);
    using Instruction16 = void(Arm::*)(u16);

    static const std::array<Instruction32, 4096> instr_arm;
    static const std::array<Instruction16, 1024> instr_thumb;

    template<uint kHash> static constexpr Instruction32 Arm_Decode();
    template<uint kHash> static constexpr Instruction16 Thumb_Decode();

    template<bool kImmediate> SHELL_INLINE u32 lsl(u32 value, u32 amount, bool flags = true);
    template<bool kImmediate> SHELL_INLINE u32 lsr(u32 value, u32 amount, bool flags = true);
    template<bool kImmediate> SHELL_INLINE u32 asr(u32 value, u32 amount, bool flags = true);
    template<bool kImmediate> SHELL_INLINE u32 ror(u32 value, u32 amount, bool flags = true);

    SHELL_INLINE u32 log(u32 op1,          bool flags = true);
    SHELL_INLINE u32 add(u32 op1, u32 op2, bool flags = true);
    SHELL_INLINE u32 sub(u32 op1, u32 op2, bool flags = true);
    SHELL_INLINE u32 adc(u32 op1, u32 op2, bool flags = true);
    SHELL_INLINE u32 sbc(u32 op1, u32 op2, bool flags = true);

    u8  readByte(u32 addr, Access access = Access::NonSequential);
    u16 readHalf(u32 addr, Access access = Access::NonSequential);
    u32 readWord(u32 addr, Access access = Access::NonSequential);

    void writeByte(u32 addr, u8  byte, Access access = Access::NonSequential);
    void writeHalf(u32 addr, u16 half, Access access = Access::NonSequential);
    void writeWord(u32 addr, u32 word, Access access = Access::NonSequential);

    u8 readIo(u32 addr);
    void writeIo(u32 addr, u8 byte);

    u32 readUnused() const;
    u32 readHalfRotate(u32 addr, Access access = Access::NonSequential);
    u32 readWordRotate(u32 addr, Access access = Access::NonSequential);
    u32 readByteSignEx(u32 addr, Access access = Access::NonSequential);
    u32 readHalfSignEx(u32 addr, Access access = Access::NonSequential);

    template<uint kState> 
    void dispatch();
    void flushHalf();
    void flushWord();

    SHELL_INLINE void idle(u64 cycles = 1);
    SHELL_INLINE void tickRam(u64 cycles);
    SHELL_INLINE void tickRom(u32 addr, u64 cycles);
    SHELL_INLINE void tickMultiply(u32 multiplier, bool sign);

    void interruptHw();
    void interruptSw();
    void interruptHandle(u64 late = 0);

    template<u32 kInstr> void Arm_BranchExchange(u32 instr);
    template<u32 kInstr> void Arm_BranchLink(u32 instr);
    template<u32 kInstr> void Arm_DataProcessing(u32 instr);
    template<u32 kInstr> void Arm_StatusTransfer(u32 instr);
    template<u32 kInstr> void Arm_Multiply(u32 instr);
    template<u32 kInstr> void Arm_MultiplyLong(u32 instr);
    template<u32 kInstr> void Arm_SingleDataTransfer(u32 instr);
    template<u32 kInstr> void Arm_HalfSignedDataTransfer(u32 instr);
    template<u32 kInstr> void Arm_BlockDataTransfer(u32 instr);
    template<u32 kInstr> void Arm_SingleDataSwap(u32 instr);
    template<u32 kInstr> void Arm_SoftwareInterrupt(u32 instr);
    template<u32 kInstr> void Arm_CoprocessorDataOperations(u32 instr);
    template<u32 kInstr> void Arm_CoprocessorDataTransfers(u32 instr);
    template<u32 kInstr> void Arm_CoprocessorRegisterTransfers(u32 instr);
    template<u32 kInstr> void Arm_Undefined(u32 instr);

    template<u16 kInstr> void Thumb_MoveShiftedRegister(u16 instr);
    template<u16 kInstr> void Thumb_AddSubtract(u16 instr);
    template<u16 kInstr> void Thumb_ImmediateOperations(u16 instr);
    template<u16 kInstr> void Thumb_AluOperations(u16 instr);
    template<u16 kInstr> void Thumb_HighRegisterOperations(u16 instr);
    template<u16 kInstr> void Thumb_LoadPcRelative(u16 instr);
    template<u16 kInstr> void Thumb_LoadStoreRegisterOffset(u16 instr);
    template<u16 kInstr> void Thumb_LoadStoreByteHalf(u16 instr);
    template<u16 kInstr> void Thumb_LoadStoreImmediateOffset(u16 instr);
    template<u16 kInstr> void Thumb_LoadStoreHalf(u16 instr);
    template<u16 kInstr> void Thumb_LoadStoreSpRelative(u16 instr);
    template<u16 kInstr> void Thumb_LoadRelativeAddress(u16 instr);
    template<u16 kInstr> void Thumb_AddOffsetSp(u16 instr);
    template<u16 kInstr> void Thumb_PushPopRegisters(u16 instr);
    template<u16 kInstr> void Thumb_LoadStoreMultiple(u16 instr);
    template<u16 kInstr> void Thumb_ConditionalBranch(u16 instr);
    template<u16 kInstr> void Thumb_SoftwareInterrupt(u16 instr);
    template<u16 kInstr> void Thumb_UnconditionalBranch(u16 instr);
    template<u16 kInstr> void Thumb_LongBranchLink(u16 instr);
    template<u16 kInstr> void Thumb_Undefined(u16 instr);

    Pipeline pipe;
    u64 target = 0;

    struct Prefetch
    {
        u64 active = 0;
        u64 cycles = 0;
    } prefetch;

    struct Interrupt
    {
        bool isServable() const;

        Event delay;
        InterruptEnable enable;
        InterruptRequest request;
        InterruptMaster master;
    } interrupt;

    WaitControl waitcnt;
    HaltControl haltcnt;
    PostFlag postflg;

    Bios bios;
    Ram<256 * 1024> ewram = {};
    Ram< 32 * 1024> iwram = {};
};

inline Arm arm;

#include "arithmetic.inl"
#include "shifts.inl"
#include "ticks.inl"
