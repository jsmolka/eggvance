#pragma once

#include <functional>
#include <string>

template<typename T>
struct Controls
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
};

template<typename T>
struct Shortcuts
{
    T reset;
    T fullscreen;
    T fr_hardware;
    T fr_custom_1;
    T fr_custom_2;
    T fr_custom_3;
    T fr_custom_4;
    T fr_unbound;
};
