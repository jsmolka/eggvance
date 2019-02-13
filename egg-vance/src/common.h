#pragma once

#include <iostream>

#define fcout() Outstream(__FUNCTION__)

struct Outstream
{
    Outstream(const char* function)
    {
        std::cout << function << " - ";
    }

    ~Outstream()
    {
        std::cout << "\n";
    }

    template<typename T>
    inline Outstream& operator<<(const T& value)
    {
        std::cout << value;

        return *this;
    }
};
