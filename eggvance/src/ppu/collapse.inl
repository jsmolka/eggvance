// Todo: object.opaque and alpha should be precalculated values
// Todo: remove mode, shouldn't be needed
// Todo: rename obj to objects
// Todo: rename bgs to backgrounds
// Todo: add color(int x) to layer
// Todo: combine win0, win1, winobj to flags

template<int begin, int end>
void PPU::collapse()
{
    int windows = mmio.dispcnt.win0 || mmio.dispcnt.win1 || mmio.dispcnt.winobj;
    int effects = mmio.bldcnt.mode != BLD_DISABLED || obj_alpha;

    std::vector<Layer> layers;
    layers.reserve(end - begin);

    for (int bg = begin; bg < end; ++bg)
    {
        if (mmio.dispcnt.bg[bg])
        {
            layers.emplace_back(
                bg,
                bgs[bg].data(),
                mmio.bgcnt[bg].priority,
                1 << bg
            );
        }
    }

    std::sort(layers.begin(), layers.end(), [](const Layer& lhs, const Layer& rhs) {
        if (lhs.prio == rhs.prio)
            return lhs.id < rhs.id;
        return lhs.prio < rhs.prio;
    });

    switch ((effects << 1) | (windows << 0))
    {
    case 0b00:
		if (obj_exist)
			collapseNN<1>(layers);
		else
			collapseNN<0>(layers);
	
        break;

    case 0b01:
		if (obj_exist)
			collapseNW<1>(layers);
		else 
			collapseNW<0>(layers);
        break;

    case 0b10:
		if (obj_exist)
			collapseBN<1>(layers);
		else
			collapseBN<0>(layers);
        break;

    case 0b11:
		if (obj_exist)
			collapseBW<1>(layers);
		else
			collapseBW<0>(layers);
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<bool has_objects>
u16 PPU::findUpperLayer(const std::vector<Layer>& layers, int x)
{
	const auto object = objects[x];
	
	for (const auto& layer : layers)
	{
		if (has_objects && object.opaque && object.precedes(layer))
			return object.color;
		
		if (layer.opaque(x))
			return layer.color(x);
	}
	if (has_objects && object.opaque)
		return object.color;
	
	return mmu.palette.readHalf(0);
}

template<bool has_objects>
void PPU::collapseNN(const std::vector<Layer>& layers)
{
	u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
	
	for (int x = 0; x < WIDTH; ++x)
	{
		scanline[x] = findUpperLayer<has_objects>(layers, x);
	}
}

template<bool win0, bool win1, bool winobj>
int PPU::windowFlags(int x)
{
	if (win0 && mmio.winh[0].contains(x))
		return mmio.winin.win0.flags;
	
	if (win1 && mmio.winh[1].contains(x))
		return mmio.winin.win1.flags;
	
	if (winobj && objects[x].window)
		return mmio.winout.winobj.flags;
	
	return mmio.winout.winout.flags;
}

template<bool has_objects, bool win0, bool win1, bool winobj>
u16 PPU::findUpperLayerWindow(const std::vector<Layer>& layers, int x, int flags)
{	
	const auto object = objects[x];
	
	for (const auto& layer : layers)
	{
		if (has_objects && flags & LF_OBJ && object.opaque && object.precedes(layer))
			return object.color;
		
		if (flags & layer.flag && layer.opaque(x))
			return layer.color(x);
	}
	if (has_objects && flags & LF_OBJ && object.opaque)
		return object.color;
	
	return mmu.palette.readHalf(0);
}

template<bool has_objects>
void PPU::collapseNW(const std::vector<Layer>& layers)
{
	int win0   = mmio.dispcnt.win0 && mmio.winv[0].contains(mmio.vcount);
	int win1   = mmio.dispcnt.win1 && mmio.winv[1].contains(mmio.vcount);
	int winobj = mmio.dispcnt.winobj && has_objects;
	
	switch ((win0 << 2) | (win1 << 1) | (winobj << 0))
	{
	case 0b000: collapseNWImpl<has_objects, 0, 0, 0>(layers); break;
	case 0b001: collapseNWImpl<has_objects, 0, 0, 1>(layers); break;
	case 0b010: collapseNWImpl<has_objects, 0, 1, 0>(layers); break;
	case 0b011: collapseNWImpl<has_objects, 0, 1, 1>(layers); break;
	case 0b100: collapseNWImpl<has_objects, 1, 0, 0>(layers); break;
	case 0b101: collapseNWImpl<has_objects, 1, 0, 1>(layers); break;
	case 0b110: collapseNWImpl<has_objects, 1, 1, 0>(layers); break;
	case 0b111: collapseNWImpl<has_objects, 1, 1, 1>(layers); break;
				
	default:
		UNREACHABLE;
		break;
	}
}

template<bool has_objects, bool win0, bool win1, bool winobj>
void PPU::collapseNWImpl(const std::vector<Layer>& layers)
{
	u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
	
	for (int x = 0; x < WIDTH; ++x)
	{
		int flags = windowFlags<win0, win1, winobj>(x);
		
		scanline[x] = findUpperLayerWindow<has_objects, win0, win1, winobj>(layers, x, flags);
	}
}

template<bool has_objects>
bool PPU::findBlendLayer(const std::vector<Layer>& layers, int x, int flags, u16& upper)
{
	const auto object = objects[x];
	
	int upper_flags = object.alpha ? LF_OBJ : mmio.bldcnt.upper.flags;
	
	upper_flags &= flags;

	for (const auto& layer : layers)
	{
		if (has_objects && object.opaque && object.precedes(layer))
		{
			upper = object.color;
			return upper_flags & LF_OBJ;
		}
		if (layer.opaque(x))
		{
			upper = layer.color(x);
			return upper_flags & layer.flag;
		}
	}
	if (has_objects && object.opaque)
	{
		upper = object.color;
		return upper_flags & LF_OBJ;
	}
	upper = mmu.palette.readHalf(0);
	return upper_flags & LF_BDP;
}

template<bool has_objects>
bool PPU::findBlendLayers(const std::vector<Layer>& layers, int x, int flags, u16& upper, u16& lower)
{
	const auto object = objects[x];
	
	int upper_flags = object.alpha ? LF_OBJ : mmio.bldcnt.upper.flags;
	int lower_flags = mmio.bldcnt.lower.flags;
	
	upper_flags &= flags;
	lower_flags &= flags;
	
	bool upper_found = false;	
	bool object_used = false;
	for (const auto& layer : layers)
	{
		if (has_objects && !object_used && object.opaque && object.precedes(layer))
		{
			if (upper_found)
			{
				lower = object.color;
				return lower_flags & LF_OBJ;
			}
			else
			{
				upper_found = true;
				upper = object.color;
				if ((upper_flags & LF_OBJ) == 0)
					return false;
			}
			object_used = true;
		}
		if (layer.opaque(x))
		{
			if (upper_found)
            {
                lower = layer.color(x);
                return lower_flags & layer.flag;
            }
            else
            {
                upper_found = true;
                upper = layer.color(x);
                if ((upper_flags & layer.flag) == 0)
                    return false;
            }
		}
	}
	if (has_objects && !object_used && object.opaque)
	{
		if (upper_found)
		{
			lower = object.color;
			return lower_flags & LF_OBJ;
		}
		else
		{
			upper_found = true;
			upper = object.color;
			if ((upper_flags & LF_OBJ) == 0)
				return false;
		}
	}
    if (upper_found)
    {
        lower = mmu.palette.readHalf(0);
        return lower_flags & LF_BDP;
    }
    else
    {
        upper = mmu.palette.readHalf(0);
        return false;
    }	
}

template<bool has_objects>
void PPU::collapseBN(const std::vector<Layer>& layers)
{
	switch (mmio.bldcnt.mode)
	{
	case 0: collapseBNImpl<has_objects, 0>(layers); break;
	case 1: collapseBNImpl<has_objects, 1>(layers); break;
	case 2: collapseBNImpl<has_objects, 2>(layers); break;
	case 3: collapseBNImpl<has_objects, 3>(layers); break;
	
	default:
		UNREACHABLE;
		break;
	}
}

template<bool has_objects, int blend_mode>
void PPU::collapseBNImpl(const std::vector<Layer>& layers)
{
	static constexpr int flags = 0xFFFF;
	
	u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
	
	for (int x = 0; x < WIDTH; ++x)
	{
		u16 upper = 0;
		u16 lower = 0;
		
		const auto& object = objects[x];
		
		if (has_objects && object.alpha && findBlendLayers<has_objects>(layers, x, flags, upper, lower))
		{
			scanline[x] = blendAlpha(upper, lower);
		}
		else
		{
			switch (blend_mode)
			{
            case BLD_ALPHA:
                if (findBlendLayers<has_objects>(layers, x, flags, upper, lower))
                    scanline[x] = blendAlpha(upper, lower);
                else
                    scanline[x] = upper;
                break;

            case BLD_WHITE:
                if (findBlendLayer<has_objects>(layers, x, flags, upper))
                    scanline[x] = blendWhite(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_BLACK:
                if (findBlendLayer<has_objects>(layers, x, flags, upper))
                    scanline[x] = blendBlack(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_DISABLED:
				scanline[x] = findUpperLayer<has_objects>(layers, x);
				break;
				
			default:
				UNREACHABLE;
				break;
			}
		}
	}
}

template<bool has_objects>
void PPU::collapseBW(const std::vector<Layer>& layers)
{
	switch (mmio.bldcnt.mode)
	{
	case 0: collapseBWImpl<has_objects, 0>(layers); break;
	case 1: collapseBWImpl<has_objects, 1>(layers); break;
	case 2: collapseBWImpl<has_objects, 2>(layers); break;
	case 3: collapseBWImpl<has_objects, 3>(layers); break;
	
	default:
		UNREACHABLE;
		break;
	}
}

template<bool has_objects, int blend_mode>
void PPU::collapseBWImpl(const std::vector<Layer>& layers)
{
	int win0   = mmio.dispcnt.win0 && mmio.winv[0].contains(mmio.vcount);
	int win1   = mmio.dispcnt.win1 && mmio.winv[1].contains(mmio.vcount);
	int winobj = mmio.dispcnt.winobj && has_objects;

	switch ((win0 << 2) | (win1 << 1) | (winobj << 0))
	{
	case 0b000: collapseBWImplImpl<has_objects, blend_mode, 0, 0, 0>(layers); break;
	case 0b001: collapseBWImplImpl<has_objects, blend_mode, 0, 0, 1>(layers); break;
	case 0b010: collapseBWImplImpl<has_objects, blend_mode, 0, 1, 0>(layers); break;
	case 0b011: collapseBWImplImpl<has_objects, blend_mode, 0, 1, 1>(layers); break;
	case 0b100: collapseBWImplImpl<has_objects, blend_mode, 1, 0, 0>(layers); break;
	case 0b101: collapseBWImplImpl<has_objects, blend_mode, 1, 0, 1>(layers); break;
	case 0b110: collapseBWImplImpl<has_objects, blend_mode, 1, 1, 0>(layers); break;
	case 0b111: collapseBWImplImpl<has_objects, blend_mode, 1, 1, 1>(layers); break;
	
	default:
		UNREACHABLE;
		break;
	}
}

template<bool win0, bool win1, bool winobj>
void PPU::windowFlagsAndEffects(int x, int& flags, int& effects)
{
	if (win0 && mmio.winh[0].contains(x))
	{
		flags = mmio.winin.win0.flags;
		effects = mmio.winin.win0.sfx;
	}
	else if (win1 && mmio.winh[1].contains(x))
	{
		flags = mmio.winin.win1.flags;
		effects = mmio.winin.win1.sfx;
	}
	else if (winobj && objects[x].window)
	{
		flags = mmio.winout.winobj.flags;
		effects = mmio.winout.winobj.sfx;
	}
	else
	{
		flags = mmio.winout.winout.flags;
		effects = mmio.winout.winout.sfx;
	}
}

#include <fmt/printf.h>

template<bool has_objects, int blend_mode, bool win0, bool win1, bool winobj>
void PPU::collapseBWImplImpl(const std::vector<Layer>& layers)
{
	u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
	
	for (int x = 0; x < WIDTH; ++x)
	{
		const auto& object = objects[x];
		
		int flags = 0;
		int effects = 0;
		windowFlagsAndEffects<win0, win1, winobj>(x, flags, effects);
	
		u16 upper = COLOR_T;
		u16 lower = COLOR_T;
		
		if (has_objects && object.alpha && findBlendLayers<has_objects>(layers, x, flags, upper, lower))
		{
			scanline[x] = blendAlpha(upper, lower);
		}
		else if (effects)
		{
			switch (blend_mode)
			{
            case BLD_ALPHA:
                if (findBlendLayers<has_objects>(layers, x, flags, upper, lower))
                    scanline[x] = blendAlpha(upper, lower);
                else
                    scanline[x] = upper;
                break;

            case BLD_WHITE:
                if (findBlendLayer<has_objects>(layers, x, flags, upper))
                    scanline[x] = blendWhite(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_BLACK:
                if (findBlendLayer<has_objects>(layers, x, flags, upper))
                    scanline[x] = blendBlack(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_DISABLED:
				scanline[x] = findUpperLayerWindow<has_objects, win0, win1, winobj>(layers, x, flags);
				break;
				
			default:
				UNREACHABLE;
				break;
			}
		}
		else
		{
            if (upper != COLOR_T)
                scanline[x] = upper;
            else
                scanline[x] = findUpperLayerWindow<has_objects, win0, win1, winobj>(layers, x, flags);
		}
	}
}
