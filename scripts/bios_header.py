import argparse
import math

HEADER = """#pragma once

#include <array>

#include "base/int.h"

inline constexpr std::array<u8, 0x{:X}> kNormmattBios =
{{
    {}
}};
"""


def main(fname):
    data = []
    with open(fname, "rb") as f:
        while (byte := f.read(1)):
            data.append(byte[0])

    last = len(data) - 1
    while not data[last]:
        last -= 1

    cols = 32
    rows = math.ceil((last + 1) / cols)

    lines = []
    for i in range(rows):
        line = []
        for j in range(cols):
            line.append("0x{:02X}".format(data[i * cols + j]))

        lines.append(", ".join(line))

    with open("bios_normmatt.h", "w") as f:
        f.write(HEADER.format(len(data), ",\n    ".join(lines)))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", dest="fname", help="Path to BIOS file", required=True)
    args = parser.parse_args()

    main(args.fname)
