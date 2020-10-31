#pragma once

#include "point.h"
#include "base/int.h"

inline constexpr auto kOrigin           = Point(0, 0);
inline constexpr auto kScreen           = Point(240, 160);
inline constexpr auto kRefreshRate      = 59.737;
inline constexpr uint kColorMask        = 0x7FFF;
inline constexpr uint kTransparent      = 0x8000;
inline constexpr uint kDecimalBits      = 8;
inline constexpr uint kTileSize         = 8;
inline constexpr uint kTileBytes16x16   = 32;
inline constexpr uint kTileBytes256x1   = 64;
inline constexpr uint kTileBytes[2]     = { kTileBytes16x16, kTileBytes256x1 };
inline constexpr uint kColorBytes       = 2;
inline constexpr uint kEntryBytes       = 2;
inline constexpr uint kBlockSize        = 256;
inline constexpr uint kBlockTiles       = kBlockSize / kTileSize;
inline constexpr uint kBlockBytes       = 0x800;
inline constexpr uint kObjectAreaTiled  = 0x1'0000;
inline constexpr uint kObjectAreaBitmap = 0x1'4000;

enum ColorMode
{
    kColorMode16x16,
    kColorMode256x1
};

enum BlendMode
{
    kBlendModeDisabled,
    kBlendModeAlpha,
    kBlendModeWhite,
    kBlendModeBlack
};

enum ObjectMode
{
    kObjectModeNormal,
    kObjectModeAlpha,
    kObjectModeWindow,
    kObjectModeInvalid
};

enum ObjectLayout
{
    kObjectLayout2d,
    kObjectLayout1d
};
