#include "gba.h"

#include <iostream>

#include "memory_map.h"

GBA::GBA()
    : running(true)
{
    arm.mmu = &mmu;

    reset();

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

GBA::~GBA()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void GBA::run(const std::string& filepath)
{
    if (!mmu.loadRom(filepath))
    {
        std::cout << __FUNCTION__ << " - Could not load ROM " << filepath << "\n";
        return;
    }

    while (running)
    {
        for (int i = 0; i < 4096; ++i)
            arm.step(); 

        drawTiles();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }
    }
}

void GBA::reset()
{
    arm.reset();
    mmu.reset();
}

void GBA::drawTiles()
{
    // Control register for background 0
    u16 bg0cnt = mmu.readHalf(REG_BG0CNT);

    // Map offset bits * 0x800 for the used map
    u32 bg_map_offset = 0x800 * (bg0cnt >> 8 & 0xFF);

    // 32 x 32 tiles (each 1 word)
    u32 bg_map = MAP_VRAM + bg_map_offset;

    // 20 vertical tiles
    for (int y = 0; y < 20; ++y)
    {
        // 30 horizontal tiles
        for (int x = 0; x < 30; ++x)
        {
            // Each tile takes up 32 words in memory
            u16 tile = mmu.readHalf(bg_map + 32 * 2 * y + 2 * x);

            u16 tile_number = tile & 0x3FF;
            u8 palette_number = tile >> 12 & 0xF;

            drawTile(8 * x, 8 * y, tile_number, palette_number);
        }
    }
}

void GBA::drawTile(int x, int y, u16 tile_number, u8 palette_number)
{
    // Control register for background 0
    u16 bg0cnt = mmu.readHalf(REG_BG0CNT);

    // Tile data offset bits * 0x4000
    u32 bg_tiles_offset = 0x4000 * (bg0cnt >> 2 & 0x3);

    u32 bg_tiles = MAP_VRAM + bg_tiles_offset;

    int x_off = 0;
    int y_off = 0;

    // 8 x 8 tiles at address of tile number
    for (int i = 0; i < 32; ++i)
    {
        // Each tile takes up 32 bytes, one byte contains two pixels
        u8 byte = mmu.readByte(bg_tiles + 32 * tile_number + i);

        if (x_off == 8)
        {
            x_off = 0;
            y_off++;
        }

        u16 c1 = mmu.readHalf(MAP_PALETTE + palette_number * 32 + 2 * (byte & 0xF));
        u16 c2 = mmu.readHalf(MAP_PALETTE + palette_number * 32 + 2 * (byte >> 4 & 0xF));

        drawPixel(x + x_off, y + y_off, c1);
        drawPixel(x + x_off + 1, y + y_off, c2);

        x_off += 2;
    }
    SDL_UpdateWindowSurface(window);
}

void GBA::drawPixel(int x, int y, u16 c)
{
    u8 r = 8 * (c & 0x1F);
    u8 g = 8 * (c >> 5 & 0x1F);
    u8 b = 8 * (c >> 10 & 0x1F);

    SDL_Rect rect = { x, y, 1, 1 };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, r, g, b));
    SDL_RenderDrawRect(renderer, &rect);
}
