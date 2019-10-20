inline KeyInput::operator int() const
{
    return keys;
}

inline KeyInput& KeyInput::operator|=(int value)
{
    keys |= value;
    return *this;
}

inline KeyInput& KeyInput::operator&=(int value)
{
    keys &= value;
    return *this;
}

template<int index>
inline u8 KeyInput::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return bits<0, 8>(keys);
    case 1: return bits<8, 2>(keys);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void KeyInput::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");
}
