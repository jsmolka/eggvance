#include "keypad.h"

#include <SDL2/SDL.h>

#include "arm/arm.h"
#include "common/config.h"
#include "common/message.h"

Keypad keypad;

Keypad::~Keypad()
{
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0)
        return;

    if (controller)
        SDL_GameControllerClose(controller);

    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void Keypad::reset()
{
    io.keycnt.reset();
    io.keyinput.reset();
    io.keyinput_raw.reset();
}

bool Keypad::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
    {
        showMessage("Cannot init controller backend.");
        return false;
    }

    if (SDL_NumJoysticks() > 0)
        controller = SDL_GameControllerOpen(0);
    else
        controller = nullptr;

    return true;
}

void Keypad::keyboardEvent(const SDL_KeyboardEvent& event)
{
    auto pair = config.controls.keyboard.find(event.keysym.sym);
    if (pair == config.controls.keyboard.end())
        return;

    handleInput(pair->second, event.state == SDL_PRESSED);
}

void Keypad::controllerAxisEvent(const SDL_ControllerAxisEvent& event)
{
    static constexpr u32 clear_ud = static_cast<int>(Button::Up) | static_cast<int>(Button::Down);
    static constexpr u32 clear_lr = static_cast<int>(Button::Left) | static_cast<int>(Button::Right);

    Button button;
    switch (event.axis)
    {
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        button = Button::L;
        break;

    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        button = Button::R;
        break;

    case SDL_CONTROLLER_AXIS_LEFTX:
        io.keyinput |= clear_lr;
        io.keyinput_raw |= clear_lr;
        button = event.value < 0 
            ? Button::Left
            : Button::Right;
        break;

    case SDL_CONTROLLER_AXIS_LEFTY:
        io.keyinput |= clear_ud;
        io.keyinput_raw |= clear_ud;
        button = event.value < 0 
            ? Button::Up
            : Button::Down;
        break;

    default:
        return;
    }
    handleInput(button, std::abs(event.value) > config.deadzone);
}

void Keypad::controllerButtonEvent(const SDL_ControllerButtonEvent& event)
{
    auto pair = config.controls.controller.find(static_cast<SDL_GameControllerButton>(event.button));
    if (pair == config.controls.controller.end())
        return;

    handleInput(pair->second, event.state == SDL_PRESSED);
}

void Keypad::controllerDeviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    else
        controller = nullptr;
}

void Keypad::handleInput(Button button, bool pressed)
{
    int mask = static_cast<int>(button);

    if (pressed)
    {
        io.keyinput &= ~mask;
        io.keyinput_raw &= ~mask;
    }
    else
    {
        io.keyinput |= mask;
        io.keyinput_raw |= mask;
    }

    handleOppositeInputs(Button::Up, Button::Down);
    handleOppositeInputs(Button::Left, Button::Right);

    if (io.keycnt.irq)
    {
        bool interrupt = io.keycnt.logic
            ? (~io.keyinput == io.keycnt.mask)
            : (~io.keyinput &  io.keycnt.mask);

        if (interrupt)
        {
            arm.interrupt(Interrupt::Keypad);
        }
    }
}

void Keypad::handleOppositeInputs(Button b1, Button b2)
{
    int mask = static_cast<int>(b1) | static_cast<int>(b2);

    if ((mask & io.keyinput_raw) == 0)
    {
        io.keyinput |= mask;
    }
    else
    {
        io.keyinput &= ~mask;
        io.keyinput |= io.keyinput_raw & mask;
    }
}
