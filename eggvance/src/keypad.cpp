#include "keypad.h"

#include <SDL2/SDL.h>

#include "arm/arm.h"
#include "common/config.h"

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
    io.keyinput = 0x3FF;
    io.keyinput_raw = 0x3FF;
    io.keycnt.reset();
}

bool Keypad::init()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
        return false;

    if (SDL_NumJoysticks() > 0)
        controller = SDL_GameControllerOpen(0);
    else
        controller = nullptr;

    return true;
}

void Keypad::keyboardEvent(const SDL_KeyboardEvent& event)
{
    auto pair = config.keyboard_map.find(event.keysym.sym);
    if (pair == config.keyboard_map.end())
        return;

    processButton(pair->second, event.state == SDL_PRESSED);
}

void Keypad::controllerAxisEvent(const SDL_ControllerAxisEvent& event)
{
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
        button = event.value < 0 
            ? Button::Left
            : Button::Right;
        break;

    case SDL_CONTROLLER_AXIS_LEFTY:
        button = event.value < 0 
            ? Button::Up
            : Button::Down;
        break;

    default:
        return;
    }
    processButton(button, std::abs(event.value) > config.deadzone);
}

void Keypad::controllerButtonEvent(const SDL_ControllerButtonEvent& event)
{
    auto pair = config.controller_map.find(static_cast<SDL_GameControllerButton>(event.button));
    if (pair == config.controller_map.end())
        return;

    processButton(pair->second, event.state == SDL_PRESSED);
}

void Keypad::controllerDeviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    else
        controller = nullptr;
}

void Keypad::processOpposingButtons(Button b1, Button b2)
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

void Keypad::processButton(Button button, bool pressed)
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

    processOpposingButtons(Button::Up, Button::Down);
    processOpposingButtons(Button::Left, Button::Right);

    if (io.keycnt.irq)
    {
        bool interrupt = io.keycnt.irq_logic
            ? (~io.keyinput == io.keycnt.keys)
            : (~io.keyinput &  io.keycnt.keys);

        if (interrupt)
        {
            arm.irq(Interrupt::Keypad);
        }
    }
}
