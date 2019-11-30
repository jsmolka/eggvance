#include "sdlinputdevice.h"

#include <stdexcept>

#include "common/config.h"

SDLInputDevice::SDLInputDevice()
{
    keyboard.a      = mapKey(config.controls.keyboard2.a     );
    keyboard.b      = mapKey(config.controls.keyboard2.b     );
    keyboard.up     = mapKey(config.controls.keyboard2.up    );
    keyboard.down   = mapKey(config.controls.keyboard2.down  );
    keyboard.left   = mapKey(config.controls.keyboard2.left  );
    keyboard.right  = mapKey(config.controls.keyboard2.right );
    keyboard.start  = mapKey(config.controls.keyboard2.start );
    keyboard.select = mapKey(config.controls.keyboard2.select);
    keyboard.l      = mapKey(config.controls.keyboard2.l     );
    keyboard.r      = mapKey(config.controls.keyboard2.r     );
}

SDLInputDevice::~SDLInputDevice()
{
    deinit();
}

void SDLInputDevice::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER))
        throw std::runtime_error("Cannot init input device");
}

void SDLInputDevice::deinit()
{
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
    {
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }
}

void SDLInputDevice::poll(u16& state)
{
    SDL_PumpEvents();

    auto sdl_state = SDL_GetKeyboardState(nullptr);

    state = 0;
    state |= sdl_state[keyboard.a     ] << BTN_A;
    state |= sdl_state[keyboard.b     ] << BTN_B;
    state |= sdl_state[keyboard.up    ] << BTN_UP;
    state |= sdl_state[keyboard.down  ] << BTN_DOWN;
    state |= sdl_state[keyboard.left  ] << BTN_LEFT;
    state |= sdl_state[keyboard.right ] << BTN_RIGHT;
    state |= sdl_state[keyboard.start ] << BTN_START;
    state |= sdl_state[keyboard.select] << BTN_SELECT;
    state |= sdl_state[keyboard.l     ] << BTN_L;
    state |= sdl_state[keyboard.r     ] << BTN_R;
    state = ~state;
}

SDL_Scancode SDLInputDevice::mapKey(Key key)
{
    switch (key)
    {
    case KEY_A: return SDL_SCANCODE_A;
    case KEY_B: return SDL_SCANCODE_B;
    case KEY_C: return SDL_SCANCODE_C;
    case KEY_D: return SDL_SCANCODE_D;
    case KEY_E: return SDL_SCANCODE_E;
    case KEY_F: return SDL_SCANCODE_F;
    case KEY_G: return SDL_SCANCODE_G;
    case KEY_H: return SDL_SCANCODE_H;
    case KEY_I: return SDL_SCANCODE_I;
    case KEY_J: return SDL_SCANCODE_J;
    case KEY_K: return SDL_SCANCODE_K;
    case KEY_L: return SDL_SCANCODE_L;
    case KEY_M: return SDL_SCANCODE_M;
    case KEY_N: return SDL_SCANCODE_N;
    case KEY_O: return SDL_SCANCODE_O;
    case KEY_P: return SDL_SCANCODE_P;
    case KEY_Q: return SDL_SCANCODE_Q;
    case KEY_R: return SDL_SCANCODE_R;
    case KEY_S: return SDL_SCANCODE_S;
    case KEY_T: return SDL_SCANCODE_T;
    case KEY_U: return SDL_SCANCODE_U;
    case KEY_V: return SDL_SCANCODE_V;
    case KEY_W: return SDL_SCANCODE_W;
    case KEY_X: return SDL_SCANCODE_X;
    case KEY_Y: return SDL_SCANCODE_Y;
    case KEY_Z: return SDL_SCANCODE_Z;
    case KEY_0: return SDL_SCANCODE_0;
    case KEY_1: return SDL_SCANCODE_1;
    case KEY_2: return SDL_SCANCODE_2;
    case KEY_3: return SDL_SCANCODE_3;
    case KEY_4: return SDL_SCANCODE_4;
    case KEY_5: return SDL_SCANCODE_5;
    case KEY_6: return SDL_SCANCODE_6;
    case KEY_7: return SDL_SCANCODE_7;
    case KEY_8: return SDL_SCANCODE_8;
    case KEY_9: return SDL_SCANCODE_9;
    }
    return SDL_SCANCODE_UNKNOWN;
}
