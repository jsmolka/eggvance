#pragma once

#define CASE1(label) case label + 0
#define CASE2(label) case label + 0: case label + 1
#define CASE4(label) case label + 0: case label + 1: case label + 2: case label + 3

#define READ1(label, reg) CASE1(label): return reg.read(addr - label)
#define READ2(label, reg) CASE2(label): return reg.read(addr - label)
#define READ4(label, reg) CASE4(label): return reg.read(addr - label)

#define WRITE1(label, reg) CASE1(label): reg.write(addr - label, byte); break
#define WRITE2(label, reg) CASE2(label): reg.write(addr - label, byte); break
#define WRITE4(label, reg) CASE4(label): reg.write(addr - label, byte); break
