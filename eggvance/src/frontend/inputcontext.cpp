#include "inputcontext.h"

#include <shell/errors.h>
#include <shell/operators.h>

#include "base/config.h"

InputContext::~InputContext()
{
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
    {
        if (controller)
            SDL_GameControllerClose(controller);

        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }
}

void InputContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER))
        throw shell::Error("Cannot init input context: {}", SDL_GetError());

    if (SDL_NumJoysticks() > 0)
        controller = SDL_GameControllerOpen(0);
}

uint InputContext::state() const
{
    constexpr auto kUdMask = (1 << Bit::Up)   | (1 << Bit::Down);
    constexpr auto kLrMask = (1 << Bit::Left) | (1 << Bit::Right);

    uint state = keyboardState() | controllerState();

    if ((state & kUdMask) == kUdMask) state &= ~kUdMask;
    if ((state & kLrMask) == kLrMask) state &= ~kLrMask;

    return state;
}

void InputContext::doDeviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    if (event.type == SDL_CONTROLLERDEVICEREMOVED)
        controller = nullptr;
}

uint InputContext::keyboardState() const
{
    auto* keyboard = SDL_GetKeyboardState(NULL);

    uint state = 0;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.a])      << Bit::A;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.b])      << Bit::B;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.up])     << Bit::Up;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.down])   << Bit::Down;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.left])   << Bit::Left;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.right])  << Bit::Right;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.start])  << Bit::Start;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.select]) << Bit::Select;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.l])      << Bit::L;
    state |= static_cast<uint>(keyboard[config.controls.keyboard.r])      << Bit::R;

    return state;
}

uint InputContext::controllerState() const
{
    if (!controller)
        return 0;

    uint state = 0;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.a))      << Bit::A;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.b))      << Bit::B;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.up))     << Bit::Up;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.down))   << Bit::Down;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.left))   << Bit::Left;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.right))  << Bit::Right;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.start))  << Bit::Start;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.select)) << Bit::Select;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.l))      << Bit::L;
    state |= static_cast<uint>(SDL_GameControllerGetButton(controller, config.controls.controller.r))      << Bit::R;

    int axis_lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    int axis_ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    int axis_tl = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    int axis_tr = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

    constexpr auto kDeadzone = 16000;

    state |= static_cast<uint>(axis_lx < 0 && std::abs(axis_lx) > kDeadzone) << Bit::Left;
    state |= static_cast<uint>(axis_lx > 0 && std::abs(axis_lx) > kDeadzone) << Bit::Right;
    state |= static_cast<uint>(axis_ly < 0 && std::abs(axis_ly) > kDeadzone) << Bit::Up;
    state |= static_cast<uint>(axis_ly > 0 && std::abs(axis_ly) > kDeadzone) << Bit::Down;
    state |= static_cast<uint>(axis_tl > kDeadzone) << Bit::L;
    state |= static_cast<uint>(axis_tr > kDeadzone) << Bit::R;

    return state;
}
