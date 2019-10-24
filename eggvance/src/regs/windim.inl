template<int limit>
void WinDim<limit>::reset()
{
    *this = {};
}

template<int limit>
bool WinDim<limit>::contains(int x) const
{
    return x >= min && x < max;
}

template<int limit>
template<int index>
inline u8 WinDim<limit>::readByte()
{
    static_assert(index <= 1, "Invalid index");
    return 0;
}

template<int limit>
template<int index>
inline void WinDim<limit>::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: reg.max = byte; break;
    case 1: reg.min = byte; break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}

template<int limit>
void WinDim<limit>::update()
{
    min = reg.min;
    max = reg.max;

    if (max > limit || max < min)
        max = limit;
}
