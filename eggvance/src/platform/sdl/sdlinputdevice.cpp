#include "sdlinputdevice.h"

#include <stdexcept>

#include "base/config.h"

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

    constexpr uint ud_mask = (1 << kBitUp  ) | (1 << kBitDown );
    constexpr uint lr_mask = (1 << kBitLeft) | (1 << kBitRight);

    if ((state & ud_mask) == ud_mask) state &= ~ud_mask;
    if ((state & lr_mask) == lr_mask) state &= ~lr_mask;

    return ~state;
}

void SDLInputDevice::processDeviceEvent(const SDL_ControllerDeviceEvent& event)
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
    state |= keyboard[controls.keyboard.a     ] << kBitA;
    state |= keyboard[controls.keyboard.b     ] << kBitB;
    state |= keyboard[controls.keyboard.up    ] << kBitUp;
    state |= keyboard[controls.keyboard.down  ] << kBitDown;
    state |= keyboard[controls.keyboard.left  ] << kBitLeft;
    state |= keyboard[controls.keyboard.right ] << kBitRight;
    state |= keyboard[controls.keyboard.start ] << kBitStart;
    state |= keyboard[controls.keyboard.select] << kBitSelect;
    state |= keyboard[controls.keyboard.l     ] << kBitL;
    state |= keyboard[controls.keyboard.r     ] << kBitR;

    return state;
}

uint SDLInputDevice::controllerState() const
{
    uint state = 0;
    if (controller)
    {
        static constexpr auto deadzone = 16000;

        state |= SDL_GameControllerGetButton(controller, controls.controller.a     ) << kBitA;
        state |= SDL_GameControllerGetButton(controller, controls.controller.b     ) << kBitB;
        state |= SDL_GameControllerGetButton(controller, controls.controller.up    ) << kBitUp;
        state |= SDL_GameControllerGetButton(controller, controls.controller.down  ) << kBitDown;
        state |= SDL_GameControllerGetButton(controller, controls.controller.left  ) << kBitLeft;
        state |= SDL_GameControllerGetButton(controller, controls.controller.right ) << kBitRight;
        state |= SDL_GameControllerGetButton(controller, controls.controller.start ) << kBitStart;
        state |= SDL_GameControllerGetButton(controller, controls.controller.select) << kBitSelect;
        state |= SDL_GameControllerGetButton(controller, controls.controller.l     ) << kBitL;
        state |= SDL_GameControllerGetButton(controller, controls.controller.r     ) << kBitR;

        int axis_lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int axis_ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
        int axis_tl = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        int axis_tr = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

        state |= (axis_lx < 0 && std::abs(axis_lx) > deadzone) << kBitLeft;
        state |= (axis_lx > 0 && std::abs(axis_lx) > deadzone) << kBitRight;
        state |= (axis_ly < 0 && std::abs(axis_ly) > deadzone) << kBitUp;
        state |= (axis_ly > 0 && std::abs(axis_ly) > deadzone) << kBitDown;
        state |= (axis_tl > deadzone) << kBitL;
        state |= (axis_tr > deadzone) << kBitR;
    }
    return state;
}

SDL_Scancode SDLInputDevice::convertKey(Key key)
{
    switch (key)
    {
    case Key::A:   return SDL_SCANCODE_A;
    case Key::B:   return SDL_SCANCODE_B;
    case Key::C:   return SDL_SCANCODE_C;
    case Key::D:   return SDL_SCANCODE_D;
    case Key::E:   return SDL_SCANCODE_E;
    case Key::F:   return SDL_SCANCODE_F;
    case Key::G:   return SDL_SCANCODE_G;
    case Key::H:   return SDL_SCANCODE_H;
    case Key::I:   return SDL_SCANCODE_I;
    case Key::J:   return SDL_SCANCODE_J;
    case Key::K:   return SDL_SCANCODE_K;
    case Key::L:   return SDL_SCANCODE_L;
    case Key::M:   return SDL_SCANCODE_M;
    case Key::N:   return SDL_SCANCODE_N;
    case Key::O:   return SDL_SCANCODE_O;
    case Key::P:   return SDL_SCANCODE_P;
    case Key::Q:   return SDL_SCANCODE_Q;
    case Key::R:   return SDL_SCANCODE_R;
    case Key::S:   return SDL_SCANCODE_S;
    case Key::T:   return SDL_SCANCODE_T;
    case Key::U:   return SDL_SCANCODE_U;
    case Key::V:   return SDL_SCANCODE_V;
    case Key::W:   return SDL_SCANCODE_W;
    case Key::X:   return SDL_SCANCODE_X;
    case Key::Y:   return SDL_SCANCODE_Y;
    case Key::Z:   return SDL_SCANCODE_Z;
    case Key::N0:  return SDL_SCANCODE_0;
    case Key::N1:  return SDL_SCANCODE_1;
    case Key::N2:  return SDL_SCANCODE_2;
    case Key::N3:  return SDL_SCANCODE_3;
    case Key::N4:  return SDL_SCANCODE_4;
    case Key::N5:  return SDL_SCANCODE_5;
    case Key::N6:  return SDL_SCANCODE_6;
    case Key::N7:  return SDL_SCANCODE_7;
    case Key::N8:  return SDL_SCANCODE_8;
    case Key::N9:  return SDL_SCANCODE_9;
    case Key::F1:  return SDL_SCANCODE_F1;
    case Key::F2:  return SDL_SCANCODE_F2;
    case Key::F3:  return SDL_SCANCODE_F3;
    case Key::F4:  return SDL_SCANCODE_F4;
    case Key::F5:  return SDL_SCANCODE_F5;
    case Key::F6:  return SDL_SCANCODE_F6;
    case Key::F7:  return SDL_SCANCODE_F7;
    case Key::F8:  return SDL_SCANCODE_F8;
    case Key::F9:  return SDL_SCANCODE_F9;
    case Key::F10: return SDL_SCANCODE_F10;
    case Key::F11: return SDL_SCANCODE_F11;
    case Key::F12: return SDL_SCANCODE_F12;
    }
    return SDL_SCANCODE_UNKNOWN;
}

SDL_GameControllerButton SDLInputDevice::convertButton(Button button)
{
    switch (button)
    {
    case Button::A:      return SDL_CONTROLLER_BUTTON_A;
    case Button::B:      return SDL_CONTROLLER_BUTTON_B;
    case Button::X:      return SDL_CONTROLLER_BUTTON_X;
    case Button::Y:      return SDL_CONTROLLER_BUTTON_Y;
    case Button::Back:   return SDL_CONTROLLER_BUTTON_BACK;
    case Button::Guide:  return SDL_CONTROLLER_BUTTON_GUIDE;
    case Button::Start:  return SDL_CONTROLLER_BUTTON_START;
    case Button::LStick: return SDL_CONTROLLER_BUTTON_LEFTSTICK;
    case Button::RStick: return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    case Button::L:      return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    case Button::R:      return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    case Button::Up:     return SDL_CONTROLLER_BUTTON_DPAD_UP;
    case Button::Down:   return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    case Button::Left:   return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    case Button::Right:  return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    }
    return SDL_CONTROLLER_BUTTON_INVALID;
}
