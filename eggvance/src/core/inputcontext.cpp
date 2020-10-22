#include "inputcontext.h"

#include "base/config.h"
#include "base/utility.h"

InputContext::~InputContext()
{
    deinit();
}

void InputContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER))
        exit("Cannot init input context");

    if (SDL_NumJoysticks() > 0)
        controller = SDL_GameControllerOpen(0);
}

uint InputContext::state() const
{
    constexpr uint ud_mask = (1 << kBitUp)   | (1 << kBitDown);
    constexpr uint lr_mask = (1 << kBitLeft) | (1 << kBitRight);

    uint state = keyboardState() | controllerState();

    if ((state & ud_mask) == ud_mask) state &= ~ud_mask;
    if ((state & lr_mask) == lr_mask) state &= ~lr_mask;

    return state;
}

void InputContext::processDeviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    else
        controller = nullptr;
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
    auto* keyboard = SDL_GetKeyboardState(NULL);

    uint state = 0;
    state |= keyboard[config.controls.keyboard.a]      << kBitA;
    state |= keyboard[config.controls.keyboard.b]      << kBitB;
    state |= keyboard[config.controls.keyboard.up]     << kBitUp;
    state |= keyboard[config.controls.keyboard.down]   << kBitDown;
    state |= keyboard[config.controls.keyboard.left]   << kBitLeft;
    state |= keyboard[config.controls.keyboard.right]  << kBitRight;
    state |= keyboard[config.controls.keyboard.start]  << kBitStart;
    state |= keyboard[config.controls.keyboard.select] << kBitSelect;
    state |= keyboard[config.controls.keyboard.l]      << kBitL;
    state |= keyboard[config.controls.keyboard.r]      << kBitR;

    return state;
}

uint InputContext::controllerState() const
{
    uint state = 0;
    if (controller)
    {
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.a)      << kBitA;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.b)      << kBitB;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.up)     << kBitUp;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.down)   << kBitDown;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.left)   << kBitLeft;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.right)  << kBitRight;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.start)  << kBitStart;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.select) << kBitSelect;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.l)      << kBitL;
        state |= SDL_GameControllerGetButton(controller, config.controls.controller.r)      << kBitR;

        int axis_lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int axis_ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
        int axis_tl = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        int axis_tr = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

        constexpr int deadzone = 16000;

        state |= (axis_lx < 0 && std::abs(axis_lx) > deadzone) << kBitLeft;
        state |= (axis_lx > 0 && std::abs(axis_lx) > deadzone) << kBitRight;
        state |= (axis_ly < 0 && std::abs(axis_ly) > deadzone) << kBitUp;
        state |= (axis_ly > 0 && std::abs(axis_ly) > deadzone) << kBitDown;
        state |= (axis_tl > deadzone) << kBitL;
        state |= (axis_tr > deadzone) << kBitR;
    }
    return state;
}
