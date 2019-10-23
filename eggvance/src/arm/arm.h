#pragma once

#include <array>

#include "common/macros.h"
#include "common/utility.h"
#include "mmu/registers/intenabled.h"
#include "mmu/registers/intmaster.h"
#include "mmu/registers/intrequest.h"
#include "mmu/registers/waitcnt.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "dmacontroller.h"
#include "registers.h"
#include "timer.h"

enum class Interrupt
{
    VBlank  = 1 <<  0,
    HBlank  = 1 <<  1,
    VMatch  = 1 <<  2,
    Timer0  = 1 <<  3,
    Timer1  = 1 <<  4,
    Timer2  = 1 <<  5,
    Timer3  = 1 <<  6,
    Serial  = 1 <<  7,
    DMA0    = 1 <<  8,
    DMA1    = 1 <<  9,
    DMA2    = 1 << 10,
    DMA3    = 1 << 11,
    Keypad  = 1 << 12,
    GamePak = 1 << 13
};

class ARM : public Registers
{
    friend class MMU;
    friend class PPU;

public:
    ARM();

    void reset();

    void run(int cycles);
    void request(Interrupt flag);

private:
    enum class Access
    {
        Seq    = 0,
        Nonseq = 1,
    };

    enum class Shift
    {
        LSL = 0b00,
        LSR = 0b01,
        ASR = 0b10,
        ROR = 0b11
    };

    enum class State
    {
        Arm   = 0,
        Thumb = 1
    };

    struct IO
    {
        IntMaster int_master;
        IntEnabled int_enabled;
        IntRequest int_request;
        WaitCnt waitcnt;
        bool halt;
    } io;

    u32 readWordRotated(u32 addr);
    u32 readHalfRotated(u32 addr);
    u32 readHalfSigned(u32 addr);

    u32 lsl(u32 value, int amount, bool& carry) const;
    u32 lsr(u32 value, int amount, bool& carry, bool immediate) const;
    u32 asr(u32 value, int amount, bool& carry, bool immediate) const;
    u32 ror(u32 value, int amount, bool& carry, bool immediate) const;
    u32 shift(Shift type, u32 value, int amount, bool& carry, bool immediate) const;

    u32 lsl(u32 value, int amount) const;
    u32 lsr(u32 value, int amount, bool immediate) const;
    u32 asr(u32 value, int amount, bool immediate) const;
    u32 ror(u32 value, int amount, bool immediate) const;
    u32 shift(Shift type, u32 value, int amount, bool immediate) const;

    inline u32 logical(u32 result, bool flags);
    inline u32 logical(u32 result, bool carry, bool flags);

    inline u32 add(u32 op1, u32 op2, bool flags);
    inline u32 sub(u32 op1, u32 op2, bool flags);

    int execute();

    template<State state>
    inline void advance();
    inline void advance();
    template<State state>
    inline void refill();

    void interrupt(u32 pc, u32 lr, PSR::Mode mode);
    void interruptHW();
    void interruptSW();

    void disasm();

    template<Access access>
    inline void cycle(u32 addr);
    inline void cycle();
    inline void cycleBooth(u32 multiplier, bool allow_ones);

    #include "isa-arm.inl"
    #include "isa-thumb.inl"

    u64 cycles;

    Timer timers[4];
    DMAController dma;

    static std::array<void(ARM::*)(u32), 4096> instr_arm;
    static std::array<void(ARM::*)(u16), 1024> instr_thumb;
};

extern ARM arm;

#include "arm.inl"
