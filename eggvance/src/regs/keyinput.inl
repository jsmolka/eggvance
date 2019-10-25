template<int index>
inline u8 KeyInput::readByte()
{
}

template<int index>
inline void KeyInput::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");
}
