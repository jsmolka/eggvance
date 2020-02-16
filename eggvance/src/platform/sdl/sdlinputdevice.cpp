#include "sdlinputdevice.h"

#include <stdexcept>

#include "common/config.h"

SDLInputDevice::~SDLInputDevice()
{
    deinit();
}

void SDLInputDevice::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER))
        throw std::runtime_error("Cannot init input device");

    if (SDL_NumJoysticks() > 0)
        controller = SDL_GameControllerOpen(0);
    else
        controller = nullptr;

    controls.keyboard = config.controls.keyboard.convert<SDL_Scancode>(convertKey);
    controls.controller = config.controls.controller.convert<SDL_GameControllerButton>(convertButton);
}

void SDLInputDevice::deinit()
{
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
    {
        if (controller)
            SDL_GameControllerClose(controller);

        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }
}

uint SDLInputDevice::state()
{
    uint state = keyboardState() | controllerState();

    constexpr uint ud_mask = (1 << SHIFT_UP  ) | (1 << SHIFT_DOWN );
    constexpr uint lr_mask = (1 << SHIFT_LEFT) | (1 << SHIFT_RIGHT);

    if ((state & ud_mask) == ud_mask) state &= ~ud_mask;
    if ((state & lr_mask) == lr_mask) state &= ~lr_mask;

    return ~state;
}

void SDLInputDevice::deviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    else
        controller = nullptr;
}

uint SDLInputDevice::keyboardState() const
{
    auto keyboard = SDL_GetKeyboardState(nullptr);

    uint state = 0;
    state |= keyboard[controls.keyboard.a     ] << SHIFT_A;
    state |= keyboard[controls.keyboard.b     ] << SHIFT_B;
    state |= keyboard[controls.keyboard.up    ] << SHIFT_UP;
    state |= keyboard[controls.keyboard.down  ] << SHIFT_DOWN;
    state |= keyboard[controls.keyboard.left  ] << SHIFT_LEFT;
    state |= keyboard[controls.keyboard.right ] << SHIFT_RIGHT;
    state |= keyboard[controls.keyboard.start ] << SHIFT_START;
    state |= keyboard[controls.keyboard.select] << SHIFT_SELECT;
    state |= keyboard[controls.keyboard.l     ] << SHIFT_L;
    state |= keyboard[controls.keyboard.r     ] << SHIFT_R;

    return state;
}

uint SDLInputDevice::controllerState() const
{
    uint state = 0;

    if (controller)
    {
        state |= SDL_GameControllerGetButton(controller, controls.controller.a     ) << SHIFT_A;
        state |= SDL_GameControllerGetButton(controller, controls.controller.b     ) << SHIFT_B;
        state |= SDL_GameControllerGetButton(controller, controls.controller.up    ) << SHIFT_UP;
        state |= SDL_GameControllerGetButton(controller, controls.controller.down  ) << SHIFT_DOWN;
        state |= SDL_GameControllerGetButton(controller, controls.controller.left  ) << SHIFT_LEFT;
        state |= SDL_GameControllerGetButton(controller, controls.controller.right ) << SHIFT_RIGHT;
        state |= SDL_GameControllerGetButton(controller, controls.controller.start ) << SHIFT_START;
        state |= SDL_GameControllerGetButton(controller, controls.controller.select) << SHIFT_SELECT;
        state |= SDL_GameControllerGetButton(controller, controls.controller.l     ) << SHIFT_L;
        state |= SDL_GameControllerGetButton(controller, controls.controller.r     ) << SHIFT_R;

        int axis_lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int axis_ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
        int axis_tl = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        int axis_tr = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

        state |= (axis_lx < 0 && std::abs(axis_lx) > config.deadzone) << SHIFT_LEFT;
        state |= (axis_lx > 0 && std::abs(axis_lx) > config.deadzone) << SHIFT_RIGHT;
        state |= (axis_ly < 0 && std::abs(axis_ly) > config.deadzone) << SHIFT_UP;
        state |= (axis_ly > 0 && std::abs(axis_ly) > config.deadzone) << SHIFT_DOWN;
        state |= (axis_tl > config.deadzone) << SHIFT_L;
        state |= (axis_tr > config.deadzone) << SHIFT_R;
    }
    return state;
}

SDL_Scancode SDLInputDevice::convertKey(Key key)
{
    switch (key)
    {
    case KEY_A:         return SDL_SCANCODE_A;
    case KEY_B:         return SDL_SCANCODE_B;
    case KEY_C:         return SDL_SCANCODE_C;
    case KEY_D:         return SDL_SCANCODE_D;
    case KEY_E:         return SDL_SCANCODE_E;
    case KEY_F:         return SDL_SCANCODE_F;
    case KEY_G:         return SDL_SCANCODE_G;
    case KEY_H:         return SDL_SCANCODE_H;
    case KEY_I:         return SDL_SCANCODE_I;
    case KEY_J:         return SDL_SCANCODE_J;
    case KEY_K:         return SDL_SCANCODE_K;
    case KEY_L:         return SDL_SCANCODE_L;
    case KEY_M:         return SDL_SCANCODE_M;
    case KEY_N:         return SDL_SCANCODE_N;
    case KEY_O:         return SDL_SCANCODE_O;
    case KEY_P:         return SDL_SCANCODE_P;
    case KEY_Q:         return SDL_SCANCODE_Q;
    case KEY_R:         return SDL_SCANCODE_R;
    case KEY_S:         return SDL_SCANCODE_S;
    case KEY_T:         return SDL_SCANCODE_T;
    case KEY_U:         return SDL_SCANCODE_U;
    case KEY_V:         return SDL_SCANCODE_V;
    case KEY_W:         return SDL_SCANCODE_W;
    case KEY_X:         return SDL_SCANCODE_X;
    case KEY_Y:         return SDL_SCANCODE_Y;
    case KEY_Z:         return SDL_SCANCODE_Z;
    case KEY_0:         return SDL_SCANCODE_0;
    case KEY_1:         return SDL_SCANCODE_1;
    case KEY_2:         return SDL_SCANCODE_2;
    case KEY_3:         return SDL_SCANCODE_3;
    case KEY_4:         return SDL_SCANCODE_4;
    case KEY_5:         return SDL_SCANCODE_5;
    case KEY_6:         return SDL_SCANCODE_6;
    case KEY_7:         return SDL_SCANCODE_7;
    case KEY_8:         return SDL_SCANCODE_8;
    case KEY_9:         return SDL_SCANCODE_9;
    case KEY_F1:        return SDL_SCANCODE_F1;
    case KEY_F2:        return SDL_SCANCODE_F2;
    case KEY_F3:        return SDL_SCANCODE_F3;
    case KEY_F4:        return SDL_SCANCODE_F4;
    case KEY_F5:        return SDL_SCANCODE_F5;
    case KEY_F6:        return SDL_SCANCODE_F6;
    case KEY_F7:        return SDL_SCANCODE_F7;
    case KEY_F8:        return SDL_SCANCODE_F8;
    case KEY_F9:        return SDL_SCANCODE_F9;
    case KEY_F10:       return SDL_SCANCODE_F10;
    case KEY_F11:       return SDL_SCANCODE_F11;
    case KEY_F12:       return SDL_SCANCODE_F12;
    case KEY_UP:        return SDL_SCANCODE_UP;
    case KEY_DOWN:      return SDL_SCANCODE_DOWN;
    case KEY_LEFT:      return SDL_SCANCODE_LEFT;
    case KEY_RIGHT:     return SDL_SCANCODE_RIGHT;
    case KEY_RETURN:    return SDL_SCANCODE_RETURN;
    case KEY_ESCAPE:    return SDL_SCANCODE_ESCAPE;
    case KEY_BACKSPACE: return SDL_SCANCODE_BACKSPACE;
    case KEY_TAB:       return SDL_SCANCODE_TAB;
    case KEY_SPACE:     return SDL_SCANCODE_SPACE;
    case KEY_CAPSLOCK:  return SDL_SCANCODE_CAPSLOCK;
    }
    return SDL_SCANCODE_UNKNOWN;
}

SDL_GameControllerButton SDLInputDevice::convertButton(Button button)
{
    switch (button)
    {
    case BTN_A:             return SDL_CONTROLLER_BUTTON_A;
    case BTN_B:             return SDL_CONTROLLER_BUTTON_B;
    case BTN_X:             return SDL_CONTROLLER_BUTTON_X;
    case BTN_Y:             return SDL_CONTROLLER_BUTTON_Y;
    case BTN_BACK:          return SDL_CONTROLLER_BUTTON_BACK;
    case BTN_GUIDE:         return SDL_CONTROLLER_BUTTON_GUIDE;
    case BTN_START:         return SDL_CONTROLLER_BUTTON_START;
    case BTN_LEFTSTICK:     return SDL_CONTROLLER_BUTTON_LEFTSTICK;
    case BTN_RIGHTSTICK:    return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    case BTN_LEFTSHOULDER:  return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    case BTN_RIGHTSHOULDER: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    case BTN_DPAD_UP:       return SDL_CONTROLLER_BUTTON_DPAD_UP;
    case BTN_DPAD_DOWN:     return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    case BTN_DPAD_LEFT:     return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    case BTN_DPAD_RIGHT:    return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    }
    return SDL_CONTROLLER_BUTTON_INVALID;
}
