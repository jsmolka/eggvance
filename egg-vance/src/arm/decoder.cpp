#include "decoder.h"

#include <array>
#include <string>

int charMask(const std::string& pattern, char mask_char)
{
    int mask = 0;
    for (char value : pattern)
    {
        mask <<= 1;
        mask |= static_cast<int>(value == mask_char);
    }
    return mask;
}

bool matches(const std::string& pattern, int bits)
{
    int mask0 = charMask(pattern, '0');
    int mask1 = charMask(pattern, '1');

    return (bits & (mask0 | mask1)) == mask1;
}

ArmInstr decodeArmHash(int hash)
{
    if (matches("101xxxxxxxxx", hash)) return ArmInstr::BranchLink;
    if (matches("100xxxxxxxxx", hash)) return ArmInstr::BlockTransfer;
    if (matches("110xxxxxxxxx", hash)) return ArmInstr::CoDataTransfer;
    if (matches("1110xxxxxxx0", hash)) return ArmInstr::CoDataOperation;
    if (matches("1110xxxxxxx1", hash)) return ArmInstr::CoRegisterTransfer;
    if (matches("1111xxxxxxxx", hash)) return ArmInstr::SWI;
    if (matches("011xxxxxxxx1", hash)) return ArmInstr::Undefined;
    if (matches("01xxxxxxxxxx", hash)) return ArmInstr::SingleTransfer;
    if (matches("000100100001", hash)) return ArmInstr::BranchExchange;
    if (matches("000000xx1001", hash)) return ArmInstr::Multiply;
    if (matches("00001xxx1001", hash)) return ArmInstr::MultiplyLong;
    if (matches("00010x001001", hash)) return ArmInstr::SingleSwap;
    if (matches("000xxxxx1xx1", hash)) return ArmInstr::HalfSignedTransfer;
    if (matches("00x10xx0xxxx", hash)) return ArmInstr::PsrTransfer;
    if (matches("00xxxxxxxxxx", hash)) return ArmInstr::DataProcessing;

    return ArmInstr::Invalid;
}

using ArmLut = std::array<ArmInstr, 4096>;

const ArmLut generateArmLut()
{
    ArmLut lut;
    for (int hash = 0; hash < lut.size(); ++hash)
        lut[hash] = decodeArmHash(hash);

    return lut;
}

inline int armHash(u32 instr)
{
    return ((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF);
}

ArmInstr decoder::decodeArm(u32 instr)
{
    static const ArmLut arm_lut = generateArmLut();

    return arm_lut[armHash(instr)];
}

ThumbInstr decodeThumbHash(int hash)
{
    if (matches("00011xxx", hash)) return ThumbInstr::AddSubImmediate;
    if (matches("000xxxxx", hash)) return ThumbInstr::MoveShiftedRegister;
    if (matches("001xxxxx", hash)) return ThumbInstr::AddSubMovCmpImmediate;
    if (matches("010000xx", hash)) return ThumbInstr::AluOperations;
    if (matches("010001xx", hash)) return ThumbInstr::HighRegisterBranchExchange;
    if (matches("01001xxx", hash)) return ThumbInstr::LoadPcRelative;
    if (matches("0101xx0x", hash)) return ThumbInstr::LoadStoreRegisterOffset;
    if (matches("0101xx1x", hash)) return ThumbInstr::LoadStoreHalfSigned;
    if (matches("011xxxxx", hash)) return ThumbInstr::LoadStoreImmediateOffset;
    if (matches("1000xxxx", hash)) return ThumbInstr::LoadStoreHalf;
    if (matches("1001xxxx", hash)) return ThumbInstr::LoadStoreSpRelative;
    if (matches("1010xxxx", hash)) return ThumbInstr::LoadAddress;
    if (matches("10110000", hash)) return ThumbInstr::AddOffsetSp;
    if (matches("1011x10x", hash)) return ThumbInstr::PushPopRegisters;
    if (matches("1100xxxx", hash)) return ThumbInstr::LoadStoreMultiple;
    if (matches("11011111", hash)) return ThumbInstr::SWI;
    if (matches("1101xxxx", hash)) return ThumbInstr::ConditionalBranch;
    if (matches("11100xxx", hash)) return ThumbInstr::UnconditionalBranch;
    if (matches("1111xxxx", hash)) return ThumbInstr::LongBranchLink;

    return ThumbInstr::Invalid;
}

using ThumbLut = std::array<ThumbInstr, 256>;

const ThumbLut generateThumbLut()
{
    ThumbLut lut;
    for (int hash = 0; hash < lut.size(); ++hash)
        lut[hash] = decodeThumbHash(hash);

    return lut;
}

inline int thumbHash(u16 instr)
{
    return instr >> 8;
}

ThumbInstr decoder::decodeThumb(u16 instr)
{
    static const ThumbLut thumb_lut = generateThumbLut();

    return thumb_lut[thumbHash(instr)];
}
