#pragma once

#include <shell/macros.h>

#include "int.h"

#define INDEXED_CASE(label, index, ...)         \
    case label + index:                         \
    {                                           \
        constexpr uint kIndex = index;          \
        constexpr uint kLabel = label + index;  \
        __VA_ARGS__;                            \
        break;                                  \
    }

#define INDEXED_CASE1(label, ...)               \
    INDEXED_CASE(label, 0, __VA_ARGS__)

#define INDEXED_CASE2(label, ...)               \
    INDEXED_CASE(label, 0, __VA_ARGS__);        \
    INDEXED_CASE(label, 1, __VA_ARGS__)

#define INDEXED_CASE4(label, ...)               \
    INDEXED_CASE(label, 0, __VA_ARGS__);        \
    INDEXED_CASE(label, 1, __VA_ARGS__);        \
    INDEXED_CASE(label, 2, __VA_ARGS__);        \
    INDEXED_CASE(label, 3, __VA_ARGS__)

#define INDEXED_CASE8(label, ...)               \
    INDEXED_CASE(label, 0, __VA_ARGS__);        \
    INDEXED_CASE(label, 1, __VA_ARGS__);        \
    INDEXED_CASE(label, 2, __VA_ARGS__);        \
    INDEXED_CASE(label, 3, __VA_ARGS__);        \
    INDEXED_CASE(label, 4, __VA_ARGS__);        \
    INDEXED_CASE(label, 5, __VA_ARGS__);        \
    INDEXED_CASE(label, 6, __VA_ARGS__);        \
    INDEXED_CASE(label, 7, __VA_ARGS__)

#define CASE01_IMPL(label, index, ...)         \
    case label + index:                        \
    {                                          \
        constexpr auto kIndex = index;         \
        constexpr auto kLabel = label + index; \
        __VA_ARGS__;                           \
        break;                                 \
    }

#define CASE02_IMPL(label, index, ...) CASE01_IMPL(label, 0, __VA_ARGS__) CASE01_IMPL(label,  1, __VA_ARGS__)
#define CASE04_IMPL(label, index, ...) CASE02_IMPL(label, 0, __VA_ARGS__) CASE02_IMPL(label,  2, __VA_ARGS__)
#define CASE08_IMPL(label, index, ...) CASE04_IMPL(label, 0, __VA_ARGS__) CASE04_IMPL(label,  4, __VA_ARGS__)
#define CASE16_IMPL(label, index, ...) CASE08_IMPL(label, 0, __VA_ARGS__) CASE08_IMPL(label,  8, __VA_ARGS__)
#define CASE32_IMPL(label, index, ...) CASE16_IMPL(label, 0, __VA_ARGS__) CASE16_IMPL(label, 16, __VA_ARGS__)

#define CASE01(label, ...) CASE01_IMPL(label, 0, __VA_ARGS__)
#define CASE02(label, ...) CASE02_IMPL(label, 0, __VA_ARGS__)
#define CASE04(label, ...) CASE04_IMPL(label, 0, __VA_ARGS__)
#define CASE08(label, ...) CASE08_IMPL(label, 0, __VA_ARGS__)
#define CASE16(label, ...) CASE16_IMPL(label, 0, __VA_ARGS__)
#define CASE32(label, ...) CASE32_IMPL(label, 0, __VA_ARGS__)
