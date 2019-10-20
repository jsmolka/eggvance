template<int index>
inline u8 DispStat::readByte()
{
    static_assert(index <= 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= vblank     << 0;
        byte |= hblank     << 1;
        byte |= vmatch     << 2;
        byte |= vblank_irq << 3;
        byte |= hblank_irq << 4;
        byte |= vmatch_irq << 5;
        break;

    case 1:
        byte = vcompare;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void DispStat::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0:
        vblank_irq = bits<3, 1>(byte);
        hblank_irq = bits<4, 1>(byte);
        vmatch_irq = bits<5, 1>(byte);
        break;

    case 1:
        vcompare = byte;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
