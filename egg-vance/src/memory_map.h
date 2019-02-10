#pragma once

#include "integer.h"

enum MemoryMap
{
    // General Internal Memory
    MAP_BIOS           = 0x00000000,
    MAP_BIOS_END       = 0x00003FFF,
    MAP_WRAM_BOARD     = 0x02000000,
    MAP_WRAM_BOARD_END = 0x0203FFFF,
    MAP_WRAM_CHIP      = 0x03000000,
    MAP_WRAM_CHIP_END  = 0x03007FFF,
    MAP_IO             = 0x04000000,
    MAP_IO_END         = 0x040003FE,

    // Internal Display Memory
    MAP_PALETTE     = 0x05000000,
    MAP_PALETTE_END = 0x050003FF,
    MAP_VRAM        = 0x06000000,
    MAP_VRAM_END    = 0x06017FFF,
    MAP_OAM         = 0x07000000,
    MAP_OAM_END     = 0x070003FF,

    // External Memory (Game Pak)
    MAP_GAMEPAK_0        = 0x08000000,  // Wait State 0
    MAP_GAMEPAK_0_END    = 0x09FFFFFF,
    MAP_GAMEPAK_1        = 0x0A000000,  // Wait State 1
    MAP_GAMEPAK_1_END    = 0x0BFFFFFF,
    MAP_GAMEPAK_2        = 0x0C000000,  // Wait State 2
    MAP_GAMEPAK_2_END    = 0x0DFFFFFF,
    MAP_GAMEPAK_SRAM     = 0x0E000000,
    MAP_GAMEPAK_SRAM_END = 0x0E00FFFF,

    // LCD I/O Registers
    REG_DISPCNT  = 0x4000000,  // LCD Control 
    REG_DISPSTAT = 0x4000004,  // General LCD Status (STAT, LYC)
    REG_VCOUNT   = 0x4000006,  // Vertical Counter (LY)
    REG_BG0CNT   = 0x4000008,  // BG0 Control
    REG_BG1CNT   = 0x400000A,  // BG1 Control
    REG_BG2CNT   = 0x400000C,  // BG2 Control
    REG_BG3CNT   = 0x400000E,  // BG3 Control
    REG_BG0VOFS  = 0x4000010,  // BG0 X-Offset
    REG_BG0HOFS  = 0x4000012,  // BG0 Y-Offset
    REG_BG1VOFS  = 0x4000014,  // BG1 X-Offset
    REG_BG1HOFS  = 0x4000016,  // BG1 Y-Offset
    REG_BG2VOFS  = 0x4000018,  // BG2 X-Offset
    REG_BG2HOFS  = 0x400001A,  // BG2 Y-Offset
    REG_BG3VOFS  = 0x400001C,  // BG3 X-Offset
    REG_BG3HOFS  = 0x400001E,  // BG3 Y-Offset
    REG_BG2PA    = 0x4000020,  // BG2 Rotation / Scaling Parameter A (dx)
    REG_BG2PB    = 0x4000022,  // BG2 Rotation / Scaling Parameter B (dmx)
    REG_BG2PC    = 0x4000024,  // BG2 Rotation / Scaling Parameter C (dy)
    REG_BG2PD    = 0x4000026,  // BG2 Rotation / Scaling Parameter D (dmy)
    REG_BG2X     = 0x4000028,  // BG2 Reference Pointer X-Coordinate
    REG_BG2Y     = 0x400002C,  // BG2 Reference Pointer Y-Coordinate
    REG_BG3PA    = 0x4000030,  // BG3 Rotation / Scaling Parameter A (dx)
    REG_BG3PB    = 0x4000032,  // BG3 Rotation / Scaling Parameter B (dmx)
    REG_BG3PC    = 0x4000034,  // BG3 Rotation / Scaling Parameter C (dy)
    REG_BG3PD    = 0x4000036,  // BG3 Rotation / Scaling Parameter D (dmy)
    REG_BG3X     = 0x4000038,  // BG3 Reference Pointer X-Coordinate
    REG_BG3Y     = 0x400003C,  // BG3 Reference Pointer Y-Coordinate
    REG_WIN0H    = 0x4000040,  // Window 0 Horizontal Dimensions
    REG_WIN1H    = 0x4000042,  // Window 1 Horizontal Dimensions
    REG_WIN0V    = 0x4000044,  // Window 0 Vertical Dimensions
    REG_WIN1V    = 0x4000046,  // Window 1 Vertical Dimensions
    REG_WININ    = 0x4000048,  // Inside of Window 0 and 1
    REG_WINOUT   = 0x400004A,  // Inside of OBJ Window and Outside of Windows
    REG_MOSAIC   = 0x400004C,  // Mosaic Size
    REG_BLDCNT   = 0x4000050,  // Color Special Effects Selection
    REG_BLDALPHA = 0x4000052,  // Alpha Blending Coefficients
    REG_BLDY     = 0x4000054,  // Brightness (Fade-In / Out) Coefficient

    // Keyboard Input
    REG_KEYINPUT = 0x4000130,  // Key Status
    REG_KEYCNT   = 0x4000132,  // Key Interrupt Control

    // Interrupt, Waitstate and Power-Down Control
    REG_IE      = 0x4000200,  // Interrupt Enable Register
    REG_IF      = 0x4000202,  // Interrupt Request Flags / IRQ Acknowledge
    REG_WAITCNT = 0x4000204,  // Game Pak Waitstate Control
    REG_IME     = 0x4000208,  // Interrupt Master Enable Register
};