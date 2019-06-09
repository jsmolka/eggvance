#include "bgbuffer.h"

#include <algorithm>

void BgBuffer::flip()
{
    page ^= 1;
}

void BgBuffer::copyPage()
{
    std::copy(
        buffer[page ^ 1].begin(),
        buffer[page ^ 1].end(),
        buffer[page].begin()
    );
}

u16 BgBuffer::operator[](int index) const
{
    return buffer[page][index];
}

u16& BgBuffer::operator[](int index)
{
    return buffer[page][index];
}
