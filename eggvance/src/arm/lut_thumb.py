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
    """Decodes thumb instruction hash"""
    if matches("00011xxxxx", x): 
        rn     = bits(0, 3, x)
        opcode = bits(3, 2, x)
        return "AddSubtract<{rn}, {opcode}>".format(
            rn=rn,
            opcode=opcode
        )

    if matches("000xxxxxxx", x): 
        offset = bits(0, 5, x)
        opcode = bits(5, 2, x)
        if opcode == 0b11:
            return "Undefined"
        return "MoveShiftedRegister<{offset: >2}, {opcode}>".format(
            offset=offset,
            opcode=opcode
        )

    if matches("001xxxxxxx", x):
        rd     = bits(2, 3, x)
        opcode = bits(5, 2, x)
        return "ImmediateOperations<{rd}, {opcode}>".format(
            rd=rd,
            opcode=opcode
        )

    if matches("010000xxxx", x):
        opcode = bits(0, 4, x)
        return "ALUOperations<{opcode: >2}>".format(
            opcode=opcode
        )

    if matches("010001xxxx", x): 
        hs     = bits(0, 1, x)
        hd     = bits(1, 1, x)
        opcode = bits(2, 2, x)
        if opcode != 0b11 and hs == hd == 0:
            return "Undefined"
        if opcode == 0b11 and hd == 1:
            return "Undefined"
        return "HighRegisterOperations<{hs}, {hd}, {opcode}>".format(
            hs=hs,
            hd=hd,
            opcode=opcode
        )

    if matches("01001xxxxx", x):
        rd = bits(2, 3, x)
        return "LoadPCRelative<{rd}>".format(
            rd=rd
        )

    if matches("0101xx0xxx", x):
        ro     = bits(0, 3, x)
        opcode = bits(4, 2, x)
        return "LoadStoreRegisterOffset<{ro}, {opcode}>".format(
            ro=ro,
            opcode=opcode
        )

    if matches("0101xx1xxx", x): 
        ro     = bits(0, 3, x)
        opcode = bits(4, 2, x)
        return "LoadStoreByteHalf<{ro}, {opcode}>".format(
            ro=ro,
            opcode=opcode
        )

    if matches("011xxxxxxx", x): 
        offset = bits(0, 5, x)
        opcode = bits(5, 2, x)
        return "LoadStoreImmediateOffset<{offset: >2}, {opcode}>".format(
            offset=offset,
            opcode=opcode
        )

    if matches("1000xxxxxx", x):
        offset = bits(0, 5, x)
        load   = bits(5, 1, x)
        return "LoadStoreHalf<{offset: >2}, {load}>".format(
            offset=offset,
            load=load
        )

    if matches("1001xxxxxx", x):
        rd   = bits(2, 3, x)
        load = bits(5, 1, x)
        return "LoadStoreSPRelative<{rd}, {load}>".format(
            rd=rd,
            load=load
        )

    if matches("1010xxxxxx", x): 
        rd     = bits(2, 3, x)
        use_sp = bits(5, 1, x)
        return "LoadRelativeAddress<{rd}, {use_sp}>".format(
            rd=rd,
            use_sp=use_sp
        )

    if matches("10110000xx", x):
        sign = bits(1, 1, x)
        return "AddOffsetSP<{sign}>".format(
            sign=sign
        )

    if matches("1011x10xxx", x):
        special = bits(2, 1, x)
        load    = bits(5, 1, x)
        return "PushPopRegisters<{special}, {load}>".format(
            special=special,
            load=load
        )
        
    if matches("1100xxxxxx", x):
        rb   = bits(2, 3, x)
        load = bits(5, 1, x)
        return "LoadStoreMultiple<{rb}, {load}>".format(
            rb=rb,
            load=load
        )

    if matches("11011111xx", x):
        return "SoftwareInterrupt"

    if matches("1101xxxxxx", x):
        condition = bits(2, 4, x)
        if condition in (0b1110, 0b1111):
            return "Undefined"
        return "ConditionalBranch<{condition: >2}>".format(
            condition=condition
        )

    if matches("11100xxxxx", x):
        return "UnconditionalBranch"

    if matches("1111xxxxxx", x):
        second = bits(5, 1, x)
        return "LongBranchLink<{second}>".format(
            second=second
        )
    
    return "Undefined"


prefix = "    &ARM::Thumb_"
template = """#include "arm.h"

ARM::HandlerThumb ARM::lut_thumb[1024] =
{{
{}
}};
"""

lut = [prefix + decode(x) for x in range(1024)]
with open("lut_thumb.cpp", "w") as output:
    output.write(template.format(",\n".join(lut)))
