#pragma once

template<int width>
inline void ARM::advance()
{
    pc += width;
}

inline void ARM::advance()
{
    pc += length();
}