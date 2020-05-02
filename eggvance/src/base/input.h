#pragma once

#include <functional>
#include <string>

enum Key
{
    KEY_NONE,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_RETURN,
    KEY_ESCAPE,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_SPACE,
    KEY_CAPSLOCK
};

enum Button
{
    BTN_NONE,
    BTN_A,
    BTN_B,
    BTN_X,
    BTN_Y,
    BTN_BACK,
    BTN_GUIDE,
    BTN_START,
    BTN_LSTICK,
    BTN_RSTICK,
    BTN_L,
    BTN_R,
    BTN_UP,
    BTN_DOWN,
    BTN_LEFT,
    BTN_RIGHT
};

template<typename T>
struct InputConfig
{
    template<typename U>
    InputConfig<U> convert(const std::function<U(T)>& converter)
    {
        InputConfig<U> result;

        result.a      = converter(a);
        result.b      = converter(b);
        result.up     = converter(up);
        result.down   = converter(down);
        result.left   = converter(left);
        result.right  = converter(right);
        result.start  = converter(start);
        result.select = converter(select);
        result.l      = converter(l);
        result.r      = converter(r);

        return result;
    }

    T a;
    T b;
    T up;
    T down;
    T left;
    T right;
    T start;
    T select;
    T l;
    T r;
};

template<typename T>
struct ShortcutConfig
{
    template<typename U>
    ShortcutConfig<U> convert(const std::function<U(T)>& converter)
    {
        ShortcutConfig<U> result;

        result.reset       = converter(reset);
        result.fullscreen  = converter(fullscreen);
        result.fr_hardware = converter(fr_hardware);
        result.fr_custom_1 = converter(fr_custom_1);
        result.fr_custom_2 = converter(fr_custom_2);
        result.fr_custom_3 = converter(fr_custom_3);
        result.fr_custom_4 = converter(fr_custom_4);
        result.fr_unbound  = converter(fr_unbound);

        return result;
    }

    T reset;
    T fullscreen;
    T fr_hardware;
    T fr_custom_1;
    T fr_custom_2;
    T fr_custom_3;
    T fr_custom_4;
    T fr_unbound;
};

Key keyByName(std::string name);
Button buttonByName(std::string name);
