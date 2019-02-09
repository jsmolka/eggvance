#pragma once

#include <string>

#include <SDL.h>

#include "arm7.h"
#include "mmu.h"

class GBA
{
public:
    GBA();
    ~GBA();

    void run(const std::string &filepath);

private:
    void reset();

    ARM7 arm;
    MMU mmu;

    bool running;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

