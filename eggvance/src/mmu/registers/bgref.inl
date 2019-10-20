template<int index>
inline u8 BgRef::readByte()
{
    static_assert(index <= 3, "Invalid index");
    return 0;
}

template<int index>
inline void BgRef::writeByte(u8 byte)
{
    static_assert(index <= 3, "Invalid index");

    if (index == 3)
        byte = signExtend<4>(byte);

    byteArray(ref)[index] = byte;
    reg = ref;
}
