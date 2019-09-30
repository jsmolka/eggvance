#include "input.h"

#include <SDL2/SDL.h>

#include "mmu/interrupt.h"

Input::Input(MMIO& mmio)
    : mmio(mmio)
{
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    controller = SDL_GameControllerOpen(0);
}

Input::~Input()
{
    if (controller)
        SDL_GameControllerClose(controller);

    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void Input::keyEvent(const SDL_KeyboardEvent& event)
{
    Button button;
    switch (event.keysym.sym)
    {
    case SDLK_u: button = BTN_A; break;
    case SDLK_h: button = BTN_B; break;
    case SDLK_f: button = BTN_SELECT; break;
    case SDLK_g: button = BTN_START; break;
    case SDLK_d: button = BTN_RIGHT; break;
    case SDLK_a: button = BTN_LEFT; break;
    case SDLK_w: button = BTN_UP; break;
    case SDLK_s: button = BTN_DOWN; break;
    case SDLK_i: button = BTN_R; break;
    case SDLK_q: button = BTN_L; break;

    default:
        return;
    }
    processInput(button, event.state);
}

void Input::controllerAxisEvent(const SDL_ControllerAxisEvent& event)
{
    static constexpr int deadzone = 16000;

    Button button;
    switch (event.axis)
    {
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        button = BTN_L;
        break;

    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        button = BTN_R;
        break;

    case SDL_CONTROLLER_AXIS_LEFTX:
        if (event.value > -deadzone && isButtonPressed(BTN_LEFT))
            processInput(BTN_LEFT, SDL_RELEASED);
        if (event.value < deadzone && isButtonPressed(BTN_RIGHT))
            processInput(BTN_RIGHT, SDL_RELEASED);
        button = event.value < 0 ? BTN_LEFT : BTN_RIGHT;
        break;

    case SDL_CONTROLLER_AXIS_LEFTY:
        if (event.value > -deadzone && isButtonPressed(BTN_UP))
            processInput(BTN_UP, SDL_RELEASED);
        if (event.value < deadzone && isButtonPressed(BTN_DOWN))
            processInput(BTN_DOWN, SDL_RELEASED);
        button = event.value < 0 ? BTN_UP : BTN_DOWN;
        break;

    default:
        return;
    }
    processInput(button, std::abs(event.value) > deadzone ? SDL_PRESSED : SDL_RELEASED);
}

void Input::controllerButtonEvent(const SDL_ControllerButtonEvent& event)
{
    Button button;
    switch (event.button)
    {
    case SDL_CONTROLLER_BUTTON_B:
    case SDL_CONTROLLER_BUTTON_Y:
        button = BTN_A;
        break;

    case SDL_CONTROLLER_BUTTON_A:
    case SDL_CONTROLLER_BUTTON_X:
        button = BTN_B;
        break;

    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        button = BTN_L;
        break;

    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        button = BTN_R;
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        button = BTN_UP;
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        button = BTN_DOWN;
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        button = BTN_LEFT;
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        button = BTN_RIGHT;
        break;

    case SDL_CONTROLLER_BUTTON_START:
        button = BTN_START;
        break;

    case SDL_CONTROLLER_BUTTON_BACK:
        button = BTN_SELECT;
        break;

    default:
        return;
    }
    processInput(button, event.state);
}

void Input::controllerDeviceEvent(const SDL_ControllerDeviceEvent& event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        controller = SDL_GameControllerOpen(event.which);
    else
        controller = nullptr;
}

bool Input::isButtonPressed(Button button) const
{
    return (mmio.keyinput & button) == 0;
}

void Input::processInput(Button button, int state)
{
    // GBA uses pull-up logic
    if (state == SDL_RELEASED)
        mmio.keyinput |= button;
    else
        mmio.keyinput &= ~button;

    if (mmio.keycnt.irq)
    {
        int pressed = ~mmio.keyinput & 0x3FF;
        
        bool interrupt = mmio.keycnt.irq_logic
            // AND mode - interrupt if all buttons are pressed
            ? (pressed == mmio.keycnt.keys)
            // OR mode - interrupt if at least one button is pressed
            : (pressed & mmio.keycnt.keys);

        if (interrupt)
        {
            Interrupt::request(IF_KEYPAD);
        }
    }
}
