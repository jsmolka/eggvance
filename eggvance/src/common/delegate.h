#pragma once

#include <type_traits>

template<typename T, typename U = void>
class Delegate
{
public:
    static_assert(std::is_class_v<T>);

    using Function = void(T::*)(U);

    Delegate()
        : Delegate(nullptr, nullptr) {}

    Delegate(T* context, Function function)
        : context(context), function(function) {}

    inline void operator()(const U& argument) const
    {
        (context->*function)(argument);
    }

    inline operator bool() const
    {
        return static_cast<bool>(context);
    }

private:
    T* context;
    Function function;
};

template<typename T>
class Delegate<T, void>
{
public:
    static_assert(std::is_class_v<T>);

    using Function = void(T::*)(void);

    Delegate()
        : Delegate(nullptr, nullptr) {}

    Delegate(T* context, Function function)
        : context(context), function(function) {}

    inline void operator()() const
    {
        (context->*function)();
    }

    inline operator bool() const
    {
        return static_cast<bool>(context);
    }

private:
    T* context;
    Function function;
};
