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
    MAP_DISPCNT  = 0x4000000,  // LCD Control 
    MAP_DISPSTAT = 0x4000004,  // General LCD Status (STAT, LYC)
    MAP_VCOUNT   = 0x4000006,  // Vertical Counter (LY)
    MAP_BG0CNT   = 0x4000008,  // BG0 Control
    MAP_BG1CNT   = 0x400000A,  // BG1 Control
    MAP_BG2CNT   = 0x400000C,  // BG2 Control
    MAP_BG3CNT   = 0x400000E,  // BG3 Control
    MAP_BG0VOFS  = 0x4000010,  // BG0 X-Offset
    MAP_BG0HOFS  = 0x4000012,  // BG0 Y-Offset
    MAP_BG1VOFS  = 0x4000014,  // BG1 X-Offset
    MAP_BG1HOFS  = 0x4000016,  // BG1 Y-Offset
    MAP_BG2VOFS  = 0x4000018,  // BG2 X-Offset
    MAP_BG2HOFS  = 0x400001A,  // BG2 Y-Offset
    MAP_BG3VOFS  = 0x400001C,  // BG3 X-Offset
    MAP_BG3HOFS  = 0x400001E,  // BG3 Y-Offset
    MAP_BG2PA    = 0x4000020,  // BG2 Rotation / Scaling Parameter A (dx)
    MAP_BG2PB    = 0x4000022,  // BG2 Rotation / Scaling Parameter B (dmx)
    MAP_BG2PC    = 0x4000024,  // BG2 Rotation / Scaling Parameter C (dy)
    MAP_BG2PD    = 0x4000026,  // BG2 Rotation / Scaling Parameter D (dmy)
    MAP_BG2X     = 0x4000028,  // BG2 Reference Pointer X-Coordinate
    MAP_BG2Y     = 0x400002C,  // BG2 Reference Pointer Y-Coordinate
    MAP_BG3PA    = 0x4000030,  // BG3 Rotation / Scaling Parameter A (dx)
    MAP_BG3PB    = 0x4000032,  // BG3 Rotation / Scaling Parameter B (dmx)
    MAP_BG3PC    = 0x4000034,  // BG3 Rotation / Scaling Parameter C (dy)
    MAP_BG3PD    = 0x4000036,  // BG3 Rotation / Scaling Parameter D (dmy)
    MAP_BG3X     = 0x4000038,  // BG3 Reference Pointer X-Coordinate
    MAP_BG3Y     = 0x400003C,  // BG3 Reference Pointer Y-Coordinate
    MAP_WIN0H    = 0x4000040,  // Window 0 Horizontal Dimensions
    MAP_WIN1H    = 0x4000042,  // Window 1 Horizontal Dimensions
    MAP_WIN0V    = 0x4000044,  // Window 0 Vertical Dimensions
    MAP_WIN1V    = 0x4000046,  // Window 1 Vertical Dimensions
    MAP_WININ    = 0x4000048,  // Inside of Window 0 and 1
    MAP_WINOUT   = 0x400004A,  // Inside of OBJ Window and Outside of Windows
    MAP_MOSAIC   = 0x400004C,  // Mosaic Size
    MAP_BLDCNT   = 0x4000050,  // Color Special Effects Selection
    MAP_BLDALPHA = 0x4000052,  // Alpha Blending Coefficients
    MAP_BLDY     = 0x4000054,  // Brightness (Fade-In / Out) Coefficient

    // Keyboard Input
    MAP_KEYINPUT = 0x4000130,  // Key Status
    MAP_KEYCNT   = 0x4000132,  // Key Interrupt Control

    // Interrupt, Waitstate and Power-Down Control
    MAP_IE      = 0x4000200,  // Interrupt Enable Register
    MAP_IF      = 0x4000202,  // Interrupt Request Flags / IRQ Acknowledge
    MAP_WAITCNT = 0x4000204,  // Game Pak Waitstate Control
    MAP_IME     = 0x4000208,  // Interrupt Master Enable Register
};