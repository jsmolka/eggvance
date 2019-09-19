template<int obj_master>
void PPU::collapse(const std::vector<Layer>& layers)
{
    int windows = mmio.dispcnt.win0 || mmio.dispcnt.win1 || mmio.dispcnt.winobj;
    int effects = mmio.bldcnt.mode != BLD_DISABLED || objects_alpha;

    switch ((effects << 1) | (windows << 0))
    {
    case 0b00: collapseNN<obj_master>(layers); break;
    case 0b01: collapseNW<obj_master>(layers); break;
    case 0b10: collapseBN<obj_master>(layers); break;
    case 0b11: collapseBW<obj_master>(layers); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<int obj_master>
void PPU::collapseNN(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        scanline[x] = upperLayer<obj_master>(layers, x);
    }
}

template<int obj_master>
void PPU::collapseNW(const std::vector<Layer>& layers)
{
    switch (possibleWindows<obj_master>())
    {
    case 0b000: collapseNW<obj_master, 0b000>(layers); break;
    case 0b001: collapseNW<obj_master, 0b001>(layers); break;
    case 0b010: collapseNW<obj_master, 0b010>(layers); break;
    case 0b011: collapseNW<obj_master, 0b011>(layers); break;
    case 0b100: collapseNW<obj_master, 0b100>(layers); break;
    case 0b101: collapseNW<obj_master, 0b101>(layers); break;
    case 0b110: collapseNW<obj_master, 0b110>(layers); break;
    case 0b111: collapseNW<obj_master, 0b111>(layers); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<int obj_master, int win_master>
void PPU::collapseNW(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        const auto& window = activeWindow<win_master>(x);

        scanline[x] = upperLayer<obj_master>(layers, x, window.flags);
    }
}

template<int obj_master>
void PPU::collapseBN(const std::vector<Layer>& layers)
{
    switch (mmio.bldcnt.mode)
    {
    case 0: collapseBN<obj_master, 0>(layers); break;
    case 1: collapseBN<obj_master, 1>(layers); break;
    case 2: collapseBN<obj_master, 2>(layers); break;
    case 3: collapseBN<obj_master, 3>(layers); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<int obj_master, int blend_mode>
void PPU::collapseBN(const std::vector<Layer>& layers)
{
    constexpr int flags = 0xFFFF;

    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        u16 upper = TRANSPARENT;
        u16 lower = TRANSPARENT;

        const auto& object = objects[x];

        if (obj_master && object.alpha && findBlendLayers<obj_master>(layers, x, flags, upper, lower))
        {
            scanline[x] = blendAlpha(upper, lower);
        }
        else
        {
            switch (blend_mode)
            {
            case BLD_ALPHA:
                if (findBlendLayers<obj_master>(layers, x, flags, upper, lower))
                    scanline[x] = blendAlpha(upper, lower);
                else
                    scanline[x] = upper;
                break;

            case BLD_WHITE:
                if (findBlendLayers<obj_master>(layers, x, flags, upper))
                    scanline[x] = blendWhite(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_BLACK:
                if (findBlendLayers<obj_master>(layers, x, flags, upper))
                    scanline[x] = blendBlack(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_DISABLED:
                scanline[x] = upperLayer<obj_master>(layers, x);
                break;

            default:
                UNREACHABLE;
                break;
            }
        }
    }
}

template<int obj_master>
void PPU::collapseBW(const std::vector<Layer>& layers)
{
    switch (mmio.bldcnt.mode)
    {
    case 0: collapseBW<obj_master, 0>(layers); break;
    case 1: collapseBW<obj_master, 1>(layers); break;
    case 2: collapseBW<obj_master, 2>(layers); break;
    case 3: collapseBW<obj_master, 3>(layers); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<int obj_master, int blend_mode>
void PPU::collapseBW(const std::vector<Layer>& layers)
{
    switch (possibleWindows<obj_master>())
    {
    case 0b000: collapseBW<obj_master, blend_mode, 0b000>(layers); break;
    case 0b001: collapseBW<obj_master, blend_mode, 0b001>(layers); break;
    case 0b010: collapseBW<obj_master, blend_mode, 0b010>(layers); break;
    case 0b011: collapseBW<obj_master, blend_mode, 0b011>(layers); break;
    case 0b100: collapseBW<obj_master, blend_mode, 0b100>(layers); break;
    case 0b101: collapseBW<obj_master, blend_mode, 0b101>(layers); break;
    case 0b110: collapseBW<obj_master, blend_mode, 0b110>(layers); break;
    case 0b111: collapseBW<obj_master, blend_mode, 0b111>(layers); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<int obj_master, int blend_mode, int win_master>
void PPU::collapseBW(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        u16 upper = TRANSPARENT;
        u16 lower = TRANSPARENT;

        const auto& object = objects[x];
        const auto& window = activeWindow<win_master>(x);

        if (obj_master && object.alpha 
            && findBlendLayers<obj_master>(layers, x, window.flags, upper, lower))
        {
            scanline[x] = blendAlpha(upper, lower);
        }
        else if (window.sfx)
        {
            switch (blend_mode)
            {
            case BLD_ALPHA:
                if (findBlendLayers<obj_master>(layers, x, window.flags, upper, lower))
                    scanline[x] = blendAlpha(upper, lower);
                else
                    scanline[x] = upper;
                break;

            case BLD_WHITE:
                if (findBlendLayers<obj_master>(layers, x, window.flags, upper))
                    scanline[x] = blendWhite(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_BLACK:
                if (findBlendLayers<obj_master>(layers, x, window.flags, upper))
                    scanline[x] = blendBlack(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_DISABLED:
                scanline[x] = upperLayer<obj_master>(layers, x, window.flags);
                break;

            default:
                UNREACHABLE;
                break;
            }
        }
        else
        {
            if (upper != TRANSPARENT)
                scanline[x] = upper;
            else
                scanline[x] = upperLayer<obj_master>(layers, x, window.flags);
        }
    }
}

template<int obj_master>
int PPU::possibleWindows() const
{
    int windows = 0;
    if (mmio.dispcnt.win0 && mmio.winv[0].contains(mmio.vcount))
        windows |= WF_WIN0;
    if (mmio.dispcnt.win1 && mmio.winv[1].contains(mmio.vcount))
        windows |= WF_WIN1;
    if (mmio.dispcnt.winobj && obj_master)
        windows |= WF_WINOBJ;

    return windows;
}

template<int win_master>
const Window& PPU::activeWindow(int x) const
{
    if (win_master & WF_WIN0 && mmio.winh[0].contains(x))
        return mmio.winin.win0;

    if (win_master & WF_WIN1 && mmio.winh[1].contains(x))
        return mmio.winin.win1;

    if (win_master & WF_WINOBJ && objects[x].window)
        return mmio.winout.winobj;

    return mmio.winout.winout;
}

template<int obj_master>
u16 PPU::upperLayer(const std::vector<Layer>& layers, int x) const
{
    const auto& object = objects[x];

    for (const auto& layer : layers)
    {
        if (obj_master && object.opaque && object.precedes(layer))
            return object.color;
        
        if (layer.opaque(x))
            return layer.color(x);
    }
    if (obj_master && object.opaque)
        return object.color;
    
    return mmu.palette.readHalf(0);
}

template<int obj_master>
u16 PPU::upperLayer(const std::vector<Layer>& layers, int x, int flags) const
{    
    const auto& object = objects[x];

    for (const auto& layer : layers)
    {
        if (obj_master && flags & LF_OBJ && object.opaque && object.precedes(layer))
            return object.color;
        
        if (flags & layer.flag && layer.opaque(x))
            return layer.color(x);
    }
    if (obj_master && flags & LF_OBJ && object.opaque)
        return object.color;
    
    return mmu.palette.readHalf(0);
}

template<int obj_master>
bool PPU::findBlendLayers(const std::vector<Layer>& layers, int x, int flags, u16& upper) const
{
    const auto& object = objects[x];
    
    int flags_upper = object.alpha ? LF_OBJ : mmio.bldcnt.upper.flags;
    
    for (const auto& layer : layers)
    {
        if (obj_master && flags & LF_OBJ && object.opaque && object.precedes(layer))
        {
            upper = object.color;
            return flags_upper & LF_OBJ;
        }
        if (flags & layer.flag && layer.opaque(x))
        {
            upper = layer.color(x);
            return flags_upper & layer.flag;
        }
    }
    if (obj_master && flags & LF_OBJ && object.opaque)
    {
        upper = object.color;
        return flags_upper & LF_OBJ;
    }
    upper = mmu.palette.readHalf(0);
    return flags_upper & LF_BDP;
}

template<int obj_master>
bool PPU::findBlendLayers(const std::vector<Layer>& layers, int x, int flags, u16& upper, u16& lower) const
{
    const auto& object = objects[x];
    
    int flags_upper = object.alpha ? LF_OBJ : mmio.bldcnt.upper.flags;
    int flags_lower = mmio.bldcnt.lower.flags;
    
    bool upper_found = false;    
    bool object_used = false;
    for (const auto& layer : layers)
    {
        if (obj_master && flags & LF_OBJ && !object_used && object.opaque && object.precedes(layer))
        {
            if (upper_found)
            {
                lower = object.color;
                return flags_lower & LF_OBJ;
            }
            else
            {
                upper_found = true;
                upper = object.color;
                if ((flags_upper & LF_OBJ) == 0)
                    return false;
            }
            object_used = true;
        }
        if (flags & layer.flag && layer.opaque(x))
        {
            if (upper_found)
            {
                lower = layer.color(x);
                return flags_lower & layer.flag;
            }
            else
            {
                upper_found = true;
                upper = layer.color(x);
                if ((flags_upper & layer.flag) == 0)
                    return false;
            }
        }
    }
    if (obj_master && flags & LF_OBJ && !object_used && object.opaque)
    {
        if (upper_found)
        {
            lower = object.color;
            return flags_lower & LF_OBJ;
        }
        else
        {
            upper_found = true;
            upper = object.color;
            if ((flags_upper & LF_OBJ) == 0)
                return false;
        }
    }
    if (upper_found)
    {
        lower = mmu.palette.readHalf(0);
        return flags_lower & LF_BDP;
    }
    else
    {
        upper = mmu.palette.readHalf(0);
        return false;
    }    
}
