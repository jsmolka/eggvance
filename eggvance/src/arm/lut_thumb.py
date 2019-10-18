prefix = "    &ARM::Thumb_"

template = """#include "arm.h"

ARM::HandlerThumb ARM::lut_thumb[256] =
{{
{0}
}};
"""


def bits(position, amount, value):
    """Retrieves bits from value"""
    return (value >> position) & ((1 << amount) - 1)


def matches(pattern, value):
    """Checks if value matches pattern"""
    for idx, char in enumerate(reversed(pattern)):
        if char == "0" and  value & (1 << idx): return False
        if char == "1" and ~value & (1 << idx): return False
    return True


def decode(x):
    if matches("00011xxx", x): return "AddSubtract"
    if matches("000xxxxx", x): return "MoveShiftedRegister"
    if matches("001xxxxx", x): return "ImmediateOperations"
    if matches("010000xx", x): return "ALUOperations"
    if matches("010001xx", x): return "HighRegisterOperations"
    if matches("01001xxx", x): return "LoadPCRelative"
    if matches("0101xx0x", x): return "LoadStoreRegisterOffset"
    if matches("0101xx1x", x): return "LoadStoreByteHalf"
    if matches("011xxxxx", x): return "LoadStoreImmediateOffset"
    if matches("1000xxxx", x): return "LoadStoreHalf"
    if matches("1001xxxx", x): return "LoadStoreSPRelative"
    if matches("1010xxxx", x): return "LoadRelativeAddress"
    if matches("10110000", x): return "AddOffsetSP"
    if matches("1011x10x", x): return "PushPopRegisters"
    if matches("1100xxxx", x): return "LoadStoreMultiple"
    if matches("11011111", x): return "SoftwareInterrupt"
    if matches("1101xxxx", x): return "ConditionalBranch"
    if matches("11100xxx", x): return "UnconditionalBranch"
    if matches("1111xxxx", x): return "LongBranchLink"
    
    return "Undefined"


def main():
    instr = [prefix + decode(x) for x in range(0, 256)]
    with open("lut_thumb.cpp", "w") as output:
        output.write(template.format(",\n".join(instr)))


if __name__ == "__main__":
    main()
