#include "gba.h"

#include <iostream>

#include "memory_map.h"

GBA::GBA()
    : running(true)
{
    // Link components
    arm.mmu = &mmu;

    // Reset components
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
        for (int i = 0; i < 10000; ++i)
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
    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 30; ++x)
        {
            const u32 bg_map = MAP_VRAM + 0x800;

            // 32 x 32 tiles (each 1 word)

            u16 tile = mmu.readHalf(bg_map + 32 * 2 * y + 2 * x);

            std::cout << "tile: " << (int)tile << "\n";

            u16 tile_number = tile & 0x3FF;
            u8 palette_number = tile >> 12 & 0xFF;

            drawTile(x, y, tile_number, palette_number);
        }
    }
}

void GBA::drawTile(int x, int y, u16 tile_number, u8 palette_number)
{
    int x_off = 0;
    int y_off = 0;

    // 8 x 8 tiles at address of tile number
    for (int i = 0; i < 32; ++i)
    {
        const u32 tile_base = MAP_VRAM + 0x4000;

        // Each tile takes up 32 bytes, one byte contains two pixels
        u8 byte = mmu.readByte(tile_base + 32 * tile_number + i);

        if (x_off == 8)
        {
            x_off = 0;
            y_off++;
        }

        mmu.readHalf(0x05000000 + palette_number * 32 + tile_number * 2);

        int c1 = (byte & 0xF) != 0 ? 255 : 0;
        int c2 = (byte >> 8 & 0xF) != 0 ? 255 : 0;

        drawPixel(8 * x + x_off, 8 * y + y_off, c1);
        drawPixel(8 * x + x_off + 1, 8 * y + y_off, c2);

        x_off += 2;
    }
    SDL_UpdateWindowSurface(window);
}

void GBA::drawPixel(int x, int y, int c)
{
    SDL_Rect rect = { x, y, 1, 1 };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, c, c, c));
    SDL_RenderDrawRect(renderer, &rect);
}
