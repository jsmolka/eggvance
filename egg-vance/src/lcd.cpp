#include "lcd.h"

#include "memory_map.h"

Lcd::Lcd()
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

Lcd::~Lcd()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void Lcd::reset()
{
    stat = {};
}

void Lcd::drawBg0()
{
    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 30; ++x)
        {
            // Each tile takes up 32 words in memory
            u16 tile = mmu->readHalf(stat.bg_map_addr[0] + 32 * 2 * y + 2 * x);

            u16 tile_number = tile & 0x3FF;
            u16 palette_number = tile >> 12 & 0xF;

            int x_off = 0;
            int y_off = 0;

            // 8x8 tiles at address of tile number
            for (int i = 0; i < 32; ++i)
            {
                u8 tile_data = mmu->readByte(stat.bg_data_addr[0] + 32 * tile_number + i);

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

void Lcd::draw(int x, int y, u16 color)
{
    u8 r = 8 * (color & 0x1F);
    u8 g = 8 * (color >> 5 & 0x1F);
    u8 b = 8 * (color >> 10 & 0x1F);

    SDL_Rect rect = { x, y, 1, 1 };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, r, g, b));
    SDL_RenderDrawRect(renderer, &rect);
}

void Lcd::redraw()
{
    SDL_UpdateWindowSurface(window);
}
