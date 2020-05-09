#pragma once

#include <functional>
#include <string>

enum class Key
{
    None,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    N1,
    N2,
    N3,
    N4,
    N5,
    N6,
    N7,
    N8,
    N9,
    N0,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12
};

enum class Button
{
    None,
    A,
    B,
    X,
    Y,
    Back,
    Guide,
    Start,
    LStick,
    RStick,
    L,
    R,
    Up,
    Down,
    Left,
    Right
};

template<typename T>
struct Controls
{
    template<typename U>
    Controls<U> convert(const std::function<U(T)>& converter)
    {
        Controls<U> result;

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
struct Shortcuts
{
    template<typename U>
    Shortcuts<U> convert(const std::function<U(T)>& converter)
    {
        Shortcuts<U> result;

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
