#include "arm.h"

#include "decode.h"
#include "rlist.h"
#include "common/bits.h"
#include "common/macros.h"

void ARM::Arm_BranchExchange(u32 instr)
{
    int rn = bits<0, 4>(instr);

    u32 addr = regs[rn];

    cpsr.t = addr & 0x1;
    pc = addr;
}

void ARM::Arm_BranchLink(u32 instr)
{
    int offset = bits<0, 24>(instr);
    int link   = bits<24, 1>(instr);

    if (link) lr = pc - 4;

    offset = signExtend<24>(offset);
    offset <<= 2;

    pc += offset;
}

void ARM::Arm_DataProcessing(u32 instr)
{
    enum class Operation
    {
        AND = 0b0000,
        EOR = 0b0001,
        SUB = 0b0010,
        RSB = 0b0011,
        ADD = 0b0100,
        ADC = 0b0101,
        SBC = 0b0110,
        RSC = 0b0111,
        TST = 0b1000,
        TEQ = 0b1001,
        CMP = 0b1010,
        CMN = 0b1011,
        ORR = 0b1100,
        MOV = 0b1101,
        BIC = 0b1110,
        MVN = 0b1111
    };

    int rd     = bits<12, 4>(instr);
    int rn     = bits<16, 4>(instr);
    int flags  = bits<20, 1>(instr);
    int opcode = bits<21, 4>(instr);
    int imm_op = bits<25, 1>(instr);

    GPR& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2 = 0;

    bool carry = cpsr.c;
    if (imm_op)
    {
        u32 value  = bits<0, 8>(instr);
        int amount = bits<8, 4>(instr);
        op2 = ror(value, amount << 1, carry, false);
    }
    else
    {
        int rm      = bits<0, 4>(instr);
        int use_reg = bits<4, 1>(instr);
        int type    = bits<5, 2>(instr);

        op2 = regs[rm];

        int amount = 0;
        if (use_reg)
        {
            int rs = bits<8, 4>(instr);
            amount = regs[rs];
            amount &= 0xFF;

            if (rn == 15) op1 += 4;
            if (rm == 15) op2 += 4;

            idle();
        }
        else
        {
            amount = bits<7, 5>(instr);
        }
        op2 = shift(Shift(type), op2, amount, carry, !use_reg);
    }

    if (rd == 15 && flags)
    {
        PSR spsr = this->spsr;
        switchMode(spsr.mode);
        cpsr = spsr;

        flags = false;
    }

    switch (Operation(opcode))
    {
    case Operation::ADD: dst = add(op1, op2, flags); break;
    case Operation::ADC: dst = adc(op1, op2, flags); break;
    case Operation::SUB: dst = sub(op1, op2, flags); break;
    case Operation::SBC: dst = sbc(op1, op2, flags); break;
    case Operation::RSB: dst = sub(op2, op1, flags); break;
    case Operation::RSC: dst = sbc(op2, op1, flags); break;
    case Operation::CMN:       add(op1, op2, flags); break;
    case Operation::CMP:       sub(op1, op2, flags); break;
    case Operation::TST:       logical(op1 &  op2, carry, flags); break;
    case Operation::TEQ:       logical(op1 ^  op2, carry, flags); break;
    case Operation::AND: dst = logical(op1 &  op2, carry, flags); break;
    case Operation::EOR: dst = logical(op1 ^  op2, carry, flags); break;
    case Operation::ORR: dst = logical(op1 |  op2, carry, flags); break;
    case Operation::MOV: dst = logical(       op2, carry, flags); break;
    case Operation::BIC: dst = logical(op1 & ~op2, carry, flags); break;
    case Operation::MVN: dst = logical(      ~op2, carry, flags); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::Arm_StatusTransfer(u32 instr)
{
    int write    = bits<21, 1>(instr);
    int use_spsr = bits<22, 1>(instr);
    int imm_op   = bits<25, 1>(instr);

    if (write)
    {
        constexpr int f_bit = 1 << 19;
        constexpr int s_bit = 1 << 18;
        constexpr int x_bit = 1 << 17;
        constexpr int c_bit = 1 << 16;

        u32 op = 0;
        if (imm_op)
        {
            u32 value  = bits<0, 8>(instr);
            int amount = bits<8, 4>(instr);
            op = rotateRight(value, amount << 1);
        }
        else
        {
            int rm = bits<0, 4>(instr);
            op = regs[rm];
        }

        u32 mask = 0;
        if (instr & f_bit) mask |= 0xFF00'0000;
        if (instr & s_bit) mask |= 0x00FF'0000;
        if (instr & x_bit) mask |= 0x0000'FF00;
        if (instr & c_bit) mask |= 0x0000'00FF;

        op &= mask;
        if (use_spsr)
        {
            spsr = (spsr & ~mask) | op;
        }
        else
        {
            if (instr & c_bit)
                switchMode(PSR::Mode(op & 0x1F));

            cpsr = (cpsr & ~mask) | op;
        }
    }
    else
    {
        int rd = bits<12, 4>(instr);
        regs[rd] = use_spsr ? spsr : cpsr;
    }
}

void ARM::Arm_Multiply(u32 instr)
{
    int rm         = bits< 0, 4>(instr);
    int rs         = bits< 8, 4>(instr);
    int rn         = bits<12, 4>(instr);
    int rd         = bits<16, 4>(instr);
    int flags      = bits<20, 1>(instr);
    int accumulate = bits<21, 1>(instr);

    u32  op1 = regs[rm];
    u32  op2 = regs[rs];
    u32  op3 = regs[rn];
    GPR& dst = regs[rd];

    dst = op1 * op2;
    if (accumulate)
    {
        dst += op3;
        idle();
    }
    logical(dst, flags);

    booth(op2, true);
}

void ARM::Arm_MultiplyLong(u32 instr)
{
    int rm         = bits< 0, 4>(instr);
    int rs         = bits< 8, 4>(instr);
    int rdl        = bits<12, 4>(instr);
    int rdh        = bits<16, 4>(instr);
    int flags      = bits<20, 1>(instr);
    int accumulate = bits<21, 1>(instr);
    int sign       = bits<22, 1>(instr);

    u64  op1  = regs[rm];
    u64  op2  = regs[rs];
    GPR& dstl = regs[rdl];
    GPR& dsth = regs[rdh];

    if (sign)
    {
        op1 = signExtend<32>(op1);
        op2 = signExtend<32>(op2);
    }

    u64 result = op1 * op2;
    if (accumulate)
    {
        result += (static_cast<u64>(dsth) << 32) | dstl;
        idle();
    }

    if (flags)
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 63;
    }

    dstl = static_cast<u32>(result);
    dsth = static_cast<u32>(result >> 32);

    booth(static_cast<u32>(op2), sign);
}

#define INDEX            \
    if (increment)       \
        addr += offset;  \
    else                 \
        addr -= offset

#define PRE_INDEX        \
    if (pre_index)       \
        INDEX

#define POST_INDEX       \
    if (!pre_index)      \
        INDEX

void ARM::Arm_SingleDataTransfer(u32 instr)
{
    int rd         = bits<12, 4>(instr);
    int rn         = bits<16, 4>(instr);
    int load       = bits<20, 1>(instr);
    int writeback  = bits<21, 1>(instr);
    int byte       = bits<22, 1>(instr);
    int increment  = bits<23, 1>(instr);
    int pre_index  = bits<24, 1>(instr);
    int reg_offset = bits<25, 1>(instr);

    writeback |= !pre_index;

    GPR& dst = regs[rd];
    u32 addr = regs[rn];

    u32 offset = 0;
    if (reg_offset)
    {
        int rm      = bits<0, 4>(instr);
        int use_reg = bits<4, 1>(instr);
        int type    = bits<5, 2>(instr);

        int amount = 0;
        if (use_reg)
        {
            int rs = bits<8, 4>(instr);
            amount = regs[rs];
            amount &= 0xFF;
        }
        else
        {
            amount = bits<7, 5>(instr);
        }
        bool carry = false;
        offset = shift(Shift(type), regs[rm], amount, carry, true);
    }
    else
    {
        offset = bits<0, 12>(instr);
    }

    PRE_INDEX;

    if (load)
    {
        dst = byte
            ? readByte(addr)
            : readWordRotated(addr);

        idle();
    }
    else
    {
        u32 value = (rd == 15)
            ? dst + 4
            : dst + 0;

        if (byte)
            writeByte(addr, value);
        else
            writeWord(addr, value);
    }

    if (writeback && (rd != rn || !load))
    {
        POST_INDEX;
        regs[rn] = addr;
    }
}

void ARM::Arm_HalfSignedDataTransfer(u32 instr)
{
    enum class Operation
    {
        SWAP  = 0b00,
        LDRH  = 0b01,
        LDRSB = 0b10,
        LDRSH = 0b11
    };

    int opcode     = bits< 5, 2>(instr);
    int rd         = bits<12, 4>(instr);
    int rn         = bits<16, 4>(instr);
    int load       = bits<20, 1>(instr);
    int writeback  = bits<21, 1>(instr);
    int imm_offset = bits<22, 1>(instr);
    int increment  = bits<23, 1>(instr);
    int pre_index  = bits<24, 1>(instr);

    writeback |= !pre_index;

    GPR& dst = regs[rd];
    u32 addr = regs[rn];

    u32 offset = 0;
    if (imm_offset)
    {
        int lower = bits<0, 4>(instr);
        int upper = bits<8, 4>(instr);
        offset = (upper << 4) | lower;
    }
    else
    {
        int rm = bits<0, 4>(instr);
        offset = regs[rm];
    }

    PRE_INDEX;

    if (load)
    {
        switch (Operation(opcode))
        {
        case Operation::SWAP:
            break;

        case Operation::LDRH:
            dst = readHalfRotated(addr);
            break;

        case Operation::LDRSB:
            dst = readByte(addr);
            dst = signExtend<8>(static_cast<u32>(dst));
            break;

        case Operation::LDRSH:
            dst = readHalfSigned(addr);
            break;

        default:
            EGG_UNREACHABLE;
            break;
        }
        idle();
    }
    else
    {
        u32 value = (rd == 15)
            ? dst + 4
            : dst + 0;

        writeHalf(addr, value);
    }

    if (writeback && (rd != rn || !load))
    {
        POST_INDEX;
        regs[rn] = addr;
    }
}

#undef INDEX
#undef PRE_INDEX
#undef POST_INDEX

void ARM::Arm_BlockDataTransfer(u32 instr)
{
    int rlist     = bits< 0, 16>(instr);
    int rn        = bits<16,  4>(instr);
    int load      = bits<20,  1>(instr);
    int writeback = bits<21,  1>(instr);
    int user_mode = bits<22,  1>(instr);
    int increment = bits<23,  1>(instr);
    int pre_index = bits<24,  1>(instr);

    u32 addr = regs[rn];
    u32 base = regs[rn];

    PSR::Mode mode = cpsr.mode;
    if (user_mode)
        switchMode(PSR::Mode::USR);

    if (rlist != 0)
    {
        if (!increment)
        {
            addr -= 4 * popcount(rlist);
            if (writeback)
            {
                regs[rn] = addr;
                writeback = false;
            }
            pre_index ^= 0x1;
        }

        if (load)
        {
            if (rlist & (1 << rn))
                writeback = false;

            for (auto x : RList(rlist))
            {
                addr += 4 * pre_index;
                regs[x] = readWord(addr);
                addr += 4 * pre_index ^ 0x4;
            }
            idle();
        }
        else
        {
            bool first = true;

            for (auto x : RList(rlist))
            {
                u32 value = x != rn
                    ? x != GPR::PC
                        ? regs[x] + 0
                        : regs[x] + 4
                    : first
                        ? base
                        : base + (increment ? 4 : -4) * popcount(rlist);

                addr += 4 * pre_index;
                writeWord(addr, value);
                addr += 4 * pre_index ^ 0x4;

                first = false;
            }
        }
    }
    else
    {
        if (load)
            pc = readWord(addr);
        else
            writeWord(addr, pc + 4);

        addr = increment
            ? addr + 0x40
            : addr - 0x40;
    }

    if (writeback)
        regs[rn] = addr;

    if (user_mode)
        switchMode(mode);
}

void ARM::Arm_SingleDataSwap(u32 instr)
{
    int rm   = bits< 0, 4>(instr);
    int rd   = bits<12, 4>(instr);
    int rn   = bits<16, 4>(instr);
    int byte = bits<22, 1>(instr);

    u32  src = regs[rm];
    GPR& dst = regs[rd];
    u32 addr = regs[rn];

    if (byte)
    {
        dst = readByte(addr);
        writeByte(addr, src);
    }
    else
    {
        dst = readWordRotated(addr);
        writeWord(addr, src);
    }
    idle();
}

void ARM::Arm_SoftwareInterrupt(u32 instr)
{
    interruptSW();
}

void ARM::Arm_CoprocessorDataOperations(u32 instr)
{
    EGG_ASSERT(false, __FUNCTION__);
}

void ARM::Arm_CoprocessorDataTransfers(u32 instr)
{
    EGG_ASSERT(false, __FUNCTION__);
}

void ARM::Arm_CoprocessorRegisterTransfers(u32 instr)
{
    EGG_ASSERT(false, __FUNCTION__);
}

void ARM::Arm_Undefined(u32 instr)
{
    EGG_ASSERT(false, __FUNCTION__);
}

void ARM::Arm_GenerateLut()
{
    for (int hash = 0; hash < instr_arm.size(); ++hash)
    {
        instr_arm[hash] = [hash]()
        {
            switch (decodeArmHash(hash))
            {
            case InstructionArm::BranchExchange: return &ARM::Arm_BranchExchange;
            case InstructionArm::BranchLink: return &ARM::Arm_BranchLink;
            case InstructionArm::DataProcessing: return &ARM::Arm_DataProcessing;
            case InstructionArm::StatusTransfer: return &ARM::Arm_StatusTransfer;
            case InstructionArm::Multiply: return &ARM::Arm_Multiply;
            case InstructionArm::MultiplyLong: return &ARM::Arm_MultiplyLong;
            case InstructionArm::SingleDataTransfer: return &ARM::Arm_SingleDataTransfer;
            case InstructionArm::HalfSignedDataTransfer: return &ARM::Arm_HalfSignedDataTransfer;
            case InstructionArm::BlockDataTransfer: return &ARM::Arm_BlockDataTransfer;
            case InstructionArm::SingleDataSwap: return &ARM::Arm_SingleDataSwap;
            case InstructionArm::SoftwareInterrupt: return &ARM::Arm_SoftwareInterrupt;
            case InstructionArm::CoprocessorDataOperations: return &ARM::Arm_CoprocessorDataOperations;
            case InstructionArm::CoprocessorDataTransfers: return &ARM::Arm_CoprocessorDataTransfers;
            case InstructionArm::CoprocessorRegisterTransfers: return &ARM::Arm_CoprocessorRegisterTransfers;
            }
            return &ARM::Arm_Undefined;
        }();
    }
}
