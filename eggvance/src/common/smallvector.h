#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

template<typename T, std::size_t N>
class SmallVector
{
public:
    using iterator = T*;

    SmallVector()
        : _size(0), _capacity(N), _heap(_stack.data()) {}

    SmallVector(const SmallVector&) = delete;
    SmallVector& operator=(const SmallVector&) = delete;

    ~SmallVector()
    {
        if (isHeapAllocated())
            delete[] _heap;
    }

    inline T& operator[](std::size_t index)
    {
        return _heap[index];
    }

    inline void clear()
    {
        if (isHeapAllocated())
        {
            delete[] _heap;
            _heap = _stack.data();
        }
        _size = 0;
        _capacity = N;
    }

    inline void push_back(T&& item)
    {
        if (_size == _capacity)
            reallocate(_capacity << 1);

        _heap[_size++] = std::move(item);
    }

    inline void push_back(const T& item)
    {
        if (_size == _capacity)
            reallocate(_capacity << 1);

        _heap[_size++] = item;
    }

    inline std::size_t size() const
    {
        return _size;
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
    inline bool isHeapAllocated() const
    {
        return _heap != _stack.data();
    }

    inline void reallocate(std::size_t size)
    {
        T* heap = new T[size];
        std::copy(begin(), end(), heap);

        if (isHeapAllocated())
            delete[] _heap;

        _heap = heap;
        _capacity = size;
    }

    std::array<T, N> _stack;
    std::size_t _capacity;
    std::size_t _size;
    T* _heap;
};
