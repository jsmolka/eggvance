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
    BTN_LEFTSTICK,
    BTN_RIGHTSTICK,
    BTN_LEFTSHOULDER,
    BTN_RIGHTSHOULDER,
    BTN_DPAD_UP,
    BTN_DPAD_DOWN,
    BTN_DPAD_LEFT,
    BTN_DPAD_RIGHT
};

template<typename T>
struct InputConfig
{
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

    template<typename U>
    InputConfig<U> map(const std::function<U(T)>& map_func)
    {
        InputConfig<U> config;

        config.a      = map_func(a);
        config.b      = map_func(b);
        config.up     = map_func(up);
        config.down   = map_func(down);
        config.left   = map_func(left);
        config.right  = map_func(right);
        config.start  = map_func(start);
        config.select = map_func(select);
        config.l      = map_func(l);
        config.r      = map_func(r);

        return config;
    }
};

template<typename T>
struct ShortcutConfig
{
    T reset;
    T fullscreen;
    T fps_default;
    T fps_option_1;
    T fps_option_2;
    T fps_option_3;
    T fps_option_4;
    T fps_unlimited;

    template<typename U>
    ShortcutConfig<U> map(const std::function<U(T)>& map_func)
    {
        ShortcutConfig<U> config;

        config.reset         = map_func(reset);
        config.fullscreen    = map_func(fullscreen);
        config.fps_default   = map_func(fps_default);
        config.fps_option_1  = map_func(fps_option_1);
        config.fps_option_2  = map_func(fps_option_2);
        config.fps_option_3  = map_func(fps_option_3);
        config.fps_option_4  = map_func(fps_option_4);
        config.fps_unlimited = map_func(fps_unlimited);

        return config;
    }
};

Key keyByName(const std::string& name);
Button buttonByName(const std::string& name);
