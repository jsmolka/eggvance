#pragma once

#include <type_traits>
#include <utility>

namespace util
{
    template<typename T>
    void reconstruct(T* instance)
    {
        static_assert(std::is_default_constructible_v<T>,
            "reconstruct<T>: T must be default constructible");

        instance->~T();
        new(instance)T();
    }

    template<typename T, typename... Args>
    void reconstruct(T* instance, Args&&... args)
    {
        static_assert(std::is_constructible_v<T, Args...>,
            "reconstruct<T, Args>: T must be constructible from Args");

        instance->~T();
        new(instance)T(std::forward<Args>(args)...);
    }
}
