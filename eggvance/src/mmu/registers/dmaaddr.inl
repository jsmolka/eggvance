template<int index>
inline u8 DMAAddr::readByte()
{
    static_assert(index <= 3, "Invalid index");
    return 0;
}

template<int index>
inline void DMAAddr::writeByte(u8 byte)
{
    static_assert(index <= 3, "Invalid index");

    byteArray(addr)[index] = byte;

    update();
}
