#include "gba.h"

#include <iostream>

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
        arm.step(); 

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
