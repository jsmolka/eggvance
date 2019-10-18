def bits(position, amount, value):
    """Retrieves bits from value"""
    return (value >> position) & ((1 << amount) - 1)


def matches(pattern, value):
    """Checks if pattern matches value"""
    for idx, char in enumerate(reversed(pattern)):
        if char == "0" and  value & (1 << idx): return False
        if char == "1" and ~value & (1 << idx): return False
    return True


def decode(x):
    """Decodes instruction hash"""
    if matches("101xxxxxxxxx", x): return "BranchLink"
    if matches("100xxxxxxxxx", x): return "BlockDataTransfer"
    if matches("110xxxxxxxxx", x): return "Undefined"  # CoprocessorDataTransfers
    if matches("1110xxxxxxx0", x): return "Undefined"  # CoprocessorDataOperations
    if matches("1110xxxxxxx1", x): return "Undefined"  # CoprocessorRegisterTransfers
    if matches("1111xxxxxxxx", x): return "SoftwareInterrupt"
    if matches("011xxxxxxxx1", x): return "Undefined"  # Actual instruction?
    if matches("01xxxxxxxxxx", x): return "SingleDataTransfer"
    if matches("000100100001", x): return "BranchExchange"
    if matches("000000xx1001", x): return "Multiply"
    if matches("00001xxx1001", x): return "MultiplyLong"
    if matches("00010x001001", x): return "SingleDataSwap"
    if matches("000xxxxx1xx1", x): return "HalfSignedDataTransfer"
    if matches("00x10xx0xxxx", x): return "StatusTransfer"
    if matches("00xxxxxxxxxx", x): return "DataProcessing"

    return "Undefined"


prefix = "    &ARM::Arm_"
template = """#include "arm.h"

ARM::HandlerArm ARM::lut_arm[4096] =
{{
{}
}};
"""

lut = [prefix + decode(x) for x in range(4096)]
with open("lut_arm.cpp", "w") as output:
    output.write(template.format(",\n".join(lut)))
