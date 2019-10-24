#include "bgcnt.h"

int BgCnt::sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

void BgCnt::reset()
{
    *this = {};
}

int BgCnt::size() const
{
    return 128 << screen_size;
}

int BgCnt::width() const
{
    return sizes[screen_size][0];
}

int BgCnt::height() const
{
    return sizes[screen_size][1];
}
