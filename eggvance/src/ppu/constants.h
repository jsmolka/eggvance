#pragma once

#include "base/int.h"

inline constexpr auto kRefreshRate      = 59.737;
inline constexpr uint kDecimalBits      = 8;
inline constexpr uint kTransparent      = 0x8000;
inline constexpr uint kColorMask        = 0x7FFF;
inline constexpr uint kColorBytes       = 2;
inline constexpr uint kTileSize         = 8;
inline constexpr uint kTileBytes[2]     = { 32, 64 };
inline constexpr uint kMapEntryBytes    = 2;
inline constexpr uint kMapBlockSize     = 256;
inline constexpr uint kMapBlockTiles    = kMapBlockSize / kTileSize;
inline constexpr uint kMapBlockBytes    = 0x800;
inline constexpr uint kTileBlockBytes   = 0x4000;
inline constexpr uint kFrameBytes       = 0xA000;
inline constexpr uint kObjectBase       = 0x1'0000;
inline constexpr uint kObjectBaseBitmap = 0x1'4000;

enum class ColorMode
{
    C16x16,
    C256x1
};

enum class BlendMode
{
    Disabled,
    Alpha,
    White,
    Black
};

enum class ObjectMode
{
    Normal,
    Alpha,
    Window,
    Invalid
};

enum class ObjectLayout
{
    TwoDim,
    OneDim
};
