#include "doublebuffer.h"

#include <algorithm>

void DoubleBuffer::flip()
{
    page ^= 1;
}

void DoubleBuffer::copyPage()
{
    std::copy(
        buffer[page ^ 1].begin(),
        buffer[page ^ 1].end(),
        buffer[page].begin()
    );
}

u16 DoubleBuffer::operator[](int index) const
{
    return buffer[page][index];
}

u16& DoubleBuffer::operator[](int index)
{
    return buffer[page][index];
}
