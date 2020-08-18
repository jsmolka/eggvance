#include "inputcontext.h"

#include "base/config.h"

uint InputContext::state() const
{
    uint state = keyboardState() | controllerState();

    constexpr uint ud_mask = (1 << kUp  ) | (1 << kDown );
    constexpr uint lr_mask = (1 << kLeft) | (1 << kRight);

    if ((state & ud_mask) == ud_mask) state &= ~ud_mask;
    if ((state & lr_mask) == lr_mask) state &= ~lr_mask;

    return ~state;
}

void InputContext::processDeviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    else
        controller = nullptr;
}

void InputContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER))
        throw std::runtime_error("Cannot init input context");

    if (SDL_NumJoysticks() > 0)
        controller = SDL_GameControllerOpen(0);
}

void InputContext::deinit()
{
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
    {
        if (controller)
            SDL_GameControllerClose(controller);

        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }
}

uint InputContext::keyboardState() const
{
    auto* keyboard = SDL_GetKeyboardState(nullptr);

    uint state = 0;
    state |= keyboard[config.controls.keyboard.a     ] << kA;
    state |= keyboard[config.controls.keyboard.b     ] << kB;
    state |= keyboard[config.controls.keyboard.up    ] << kUp;
    state |= keyboard[config.controls.keyboard.down  ] << kDown;
    state |= keyboard[config.controls.keyboard.left  ] << kLeft;
    state |= keyboard[config.controls.keyboard.right ] << kRight;
    state |= keyboard[config.controls.keyboard.start ] << kStart;
    state |= keyboard[config.controls.keyboard.select] << kSelect;
    state |= keyboard[config.controls.keyboard.l     ] << kL;
    state |= keyboard[config.controls.keyboard.r     ] << kR;

    return state;
}

uint InputContext::controllerState() const
{
    uint state = 0;
    if (controller)
    {
        static constexpr int deadzone = 16000;

        state |= SDL_GameControllerGetButton(controller, config.controls.controller.a     ) << kA;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.b     ) << kB;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.up    ) << kUp;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.down  ) << kDown;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.left  ) << kLeft;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.right ) << kRight;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.start ) << kStart;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.select) << kSelect;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.l     ) << kL;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.r     ) << kR;

        int axis_lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int axis_ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
        int axis_tl = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        int axis_tr = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

        state |= (axis_lx < 0 && std::abs(axis_lx) > deadzone) << kLeft;
        state |= (axis_lx > 0 && std::abs(axis_lx) > deadzone) << kRight;
        state |= (axis_ly < 0 && std::abs(axis_ly) > deadzone) << kUp;
        state |= (axis_ly > 0 && std::abs(axis_ly) > deadzone) << kDown;
        state |= (axis_tl > deadzone) << kL;
        state |= (axis_tr > deadzone) << kR;
    }
    return state;
}
