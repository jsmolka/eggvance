#include "lcd.h"

#include "memory_map.h"

LCD::LCD()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "egg-vance",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        240,
        160,
        SDL_WINDOW_OPENGL
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
    surface = SDL_GetWindowSurface(window);
}

LCD::~LCD()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void LCD::reset()
{
    stat = {};
}

void LCD::drawBg0()
{
    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 30; ++x)
        {
            // Each tile takes up 32 words in memory
            u16 tile = mmu->readHalf(stat.bg_tile_map_addr[0] + 32 * 2 * y + 2 * x);

            u16 tile_number = tile & 0x3FF;
            u16 palette_number = tile >> 12 & 0xF;

            int x_off = 0;
            int y_off = 0;

            // 8x8 tiles at address of tile number
            for (int i = 0; i < 32; ++i)
            {
                u8 tile_data = mmu->readByte(stat.bg_tile_data_addr[0] + 32 * tile_number + i);

                if (x_off == 8)
                {
                    x_off = 0;
                    y_off++;
                }

                u16 color_lhs = mmu->readHalf(MAP_PALETTE + 32 * palette_number + 2 * (tile_data & 0xF));
                u16 color_rhs = mmu->readHalf(MAP_PALETTE + 32 * palette_number + 2 * (tile_data >> 4 & 0xF));

                draw(8 * x + x_off, 8 * y + y_off, color_lhs);
                draw(8 * x + x_off + 1, 8 * y + y_off, color_rhs);

                x_off += 2;
            }
        }
    }
    redraw();
}

LCD::Rgb LCD::decodeColor(u16 color)
{
    return Rgb 
    {
        static_cast<u8>(8 * (color & 0x1F)),
        static_cast<u8>(8 * (color >> 5 & 0x1F)),
        static_cast<u8>(8 * (color >> 10 & 0x1F))
    };
}

void LCD::draw(int x, int y, u16 color)
{
    Rgb rgb = decodeColor(color);

    SDL_Rect rect = { x, y, 1, 1 };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, rgb.r, rgb.g, rgb.b));
    SDL_RenderDrawRect(renderer, &rect);
}

void LCD::redraw()
{
    SDL_UpdateWindowSurface(window);
}
