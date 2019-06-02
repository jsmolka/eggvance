#include "decoder.h"

#include <array>

using namespace decoder;

using ArmLut = std::array<ArmInstr, 4096>;
using ThumbLut = std::array<ThumbInstr, 256>;

constexpr ArmInstr decodeArmHash(u16 hash)
{
    if ((hash >> 9) == 0b000000000101)
    {
        return ArmInstr::BranchLink;
    }
    if ((hash >> 9) == 0b000000000100)
    {
        return ArmInstr::BlockTransfer;
    }
    if ((hash >> 9) == 0b000000000110)
    {
        return ArmInstr::CoDataTransfer;
    }
    if ((hash >> 8) == 0b000000001110)
    {
        return (hash & 0x1) ? ArmInstr::CoRegisterTransfer : ArmInstr::CoDataOperation;
    }
    if ((hash >> 8) == 0b000000001111)
    {
        return ArmInstr::SWI;
    }
    if ((hash >> 10) == 0b000000000001)
    {
        // Could also be the undefined instruction

        return ArmInstr::SingleTransfer;
    }
    if ((hash >> 10) == 0b000000000000)
    {
        if ((hash >> 0) == 0b000100100001)
        {
            return ArmInstr::BranchExchange;
        }
        if ((((hash >> 6) & 0x3F) == 0b000000000000)
            && (((hash >> 0) & 0xF) == 0b000000001001))
        {
            return ArmInstr::Multiply;
        }
        if ((((hash >> 7) & 0x3F) == 0b000000000001)
            && (((hash >> 0) & 0xF) == 0b000000001001))
        {
            return ArmInstr::MultiplyLong;
        }
        if ((((hash >> 7) & 0x3F) == 0b000000000010)
            && (((hash >> 4) & 0x3) == 0b000000000000)
            && (((hash >> 0) & 0xF) == 0b000000001001))
        {
            return ArmInstr::SingleSwap;
        }
        if ((((hash >> 9) & 0x7) == 0b000000000000)
            && (((hash >> 0) & 0x1) == 0b000000000001)
            && (((hash >> 3) & 0x1) == 0b000000000001))
        {
            return ArmInstr::HalfSignedTransfer;
        }

        switch ((hash >> 5) & 0xF)
        {
        case 0b1000:  // TST
        case 0b1001:  // TEQ
        case 0b1010:  // CMP
        case 0b1011:  // CMN
            return (hash & 0x10) ? ArmInstr::DataProcessing : ArmInstr::PsrTransfer;
        }
        return ArmInstr::DataProcessing;
    }
    return ArmInstr::Invalid;
}

constexpr ThumbInstr decodeThumbHash(u8 hash)
{
    if ((hash >> 3) == 0b00000011)
    {
        return ThumbInstr::AddSubImmediate;
    }
    if ((hash >> 5) == 0b00000000)
    {
        return ThumbInstr::MoveShiftedRegister;
    }
    if ((hash >> 5) == 0b00000001)
    {
        return ThumbInstr::AddSubMovCmpImmediate;
    }
    if ((hash >> 2) == 0b00010000)
    {
        return ThumbInstr::AluOperations;
    }
    if ((hash >> 2) == 0b00010001)
    {
        return ThumbInstr::HighRegisterBranchExchange;
    }
    if ((hash >> 3) == 0b00001001)
    {
        return ThumbInstr::LoadPcRelative;
    }
    if ((hash >> 4) == 0b00000101)
    {
        return (hash & 0b00000010) ? ThumbInstr::LoadStoreHalfSigned : ThumbInstr::LoadStoreRegisterOffset;
    }
    if ((hash >> 5) == 0b00000011)
    {
        return ThumbInstr::LoadStoreImmediateOffset;
    }
    if ((hash >> 4) == 0b00001000)
    {
        return ThumbInstr::LoadStoreHalf;
    }
    if ((hash >> 4) == 0b00001001)
    {
        return ThumbInstr::LoadStoreSpRelative;
    }
    if ((hash >> 4) == 0b00001010)
    {
        return ThumbInstr::LoadAddress;
    }
    if ((hash >> 0) == 0b10110000)
    {
        return ThumbInstr::AddOffsetSp;
    }
    if ((hash >> 4) == 0b00001011)
    {
        return ThumbInstr::PushPopRegisters;
    }
    if ((hash >> 4) == 0b00001100)
    {
        return ThumbInstr::LoadStoreMultiple;
    }
    if ((hash >> 0) == 0b11011111)
    {
        return ThumbInstr::SWI;
    }
    if ((hash >> 4) == 0b00001101)
    {
        return ThumbInstr::ConditionalBranch;
    }
    if ((hash >> 3) == 0b00011100)
    {
        return ThumbInstr::UnconditionalBranch;
    }
    if ((hash >> 4) == 0b00001111)
    {
        return ThumbInstr::LongBranchLink;
    }
    return ThumbInstr::Invalid;
}

constexpr ArmLut generateArmLut()
{
    ArmLut lut = { ArmInstr::Invalid };
    for (std::size_t hash = 0; hash < lut.size(); ++hash)
        lut[hash] = decodeArmHash(static_cast<u16>(hash));

    return lut;
}

constexpr ThumbLut generateThumbLut()
{
    ThumbLut lut = { ThumbInstr::Invalid };
    for (std::size_t hash = 0; hash < lut.size(); ++hash)
        lut[hash] = decodeThumbHash(static_cast<u8>(hash));

    return lut;
}

static constexpr ArmLut arm_lut = generateArmLut();

inline u16 armHash(u32 instr)
{
    return static_cast<u16>(((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF));
}

ArmInstr decoder::decodeArm(u32 instr)
{
    return arm_lut[armHash(instr)];
}

static constexpr ThumbLut thumb_lut = generateThumbLut();

inline u8 thumbHash(u16 instr)
{
    return static_cast<u8>(instr >> 8);
}

ThumbInstr decoder::decodeThumb(u16 instr)
{
    return thumb_lut[thumbHash(instr)];
}
