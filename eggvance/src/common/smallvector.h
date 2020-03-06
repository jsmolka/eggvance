#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

template<typename T>
class SmallVectorBase
{
public:
    using iterator = T*;

    SmallVectorBase(T* stack, std::size_t stack_size)
        : _heap(nullptr)
        , _stack(stack)
        , _size(0)
        , _capacity(stack_size)
        , _stack_size(stack_size) {}

    SmallVectorBase(const SmallVectorBase&) = delete;
    SmallVectorBase& operator=(const SmallVectorBase&) = delete;

    ~SmallVectorBase()
    {
        if (_heap != _stack)
            delete[] _heap;
    }

    inline T& operator[](std::size_t index) const
    {
        return _heap[index];
    }

    inline T& operator[](std::size_t index)
    {
        return _heap[index];
    }

    inline std::size_t size() const
    {
        return _size;
    }

    inline void clear()
    {
        if (_heap != _stack)
        {
            delete[] _heap;
            _heap = _stack;
        }
        _size = 0;
        _capacity = _stack_size;
    }

    inline void push_back(T&& item)
    {
        if (_size == _capacity)
            realloc(_capacity << 1);

        _heap[_size++] = std::move(item);
    }

    inline void push_back(const T& item)
    {
        if (_size == _capacity)
            realloc(_capacity << 1);

        _heap[_size++] = item;
    }

    inline iterator begin()
    {
        return &_heap[0];
    }

    inline iterator end()
    {
        return &_heap[_size];
    }

private:
    inline void realloc(std::size_t size)
    {
        T* heap = new T[size];
        std::copy(begin(), end(), heap);

        if (_heap != _stack)
            delete[] _heap;

        _heap = heap;
        _capacity = size;
    }

    T* _heap;
    T* _stack;

    std::size_t _size;
    std::size_t _capacity;
    std::size_t _stack_size;
};

template<typename T, std::size_t N>
class SmallVector : public SmallVectorBase<T>
{
public:
    SmallVector()
        : SmallVectorBase(stack.data(), N) {}

private:
    std::array<T, N> stack;
};
