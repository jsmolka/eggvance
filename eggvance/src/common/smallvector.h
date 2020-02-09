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
        : size(0), capacity(N), heap(stack.data()) {}

    ~SmallVector()
    {
        if (heap != stack.data())
            delete[] heap;
    }

    SmallVector(const SmallVector&) = delete;
    SmallVector& operator=(const SmallVector&) = delete;

    inline T& operator[](std::size_t index)
    {
        return heap[index];
    }

    inline void clear()
    {
        if (heap != stack.data())
        {
            delete[] heap;
            heap = stack.data();
        }
        size = 0;
        capacity = N;
    }

    inline void push_back(T&& item)
    {
        if (size == capacity)
            grow();

        heap[size++] = std::move(item);
    }

    inline void push_back(const T& item)
    {
        if (size == capacity)
            grow();

        heap[size++] = item;
    }

    inline iterator begin()
    {
        return &heap[0];
    }

    inline iterator end()
    {
        return &heap[size];
    }

    std::size_t size;

private:
    inline void grow()
    {
        capacity <<= 1;
        T* data = new T[capacity];
        std::copy(begin(), end(), data);

        if (heap != stack.data())
            delete[] heap;

        heap = data;
    }

    std::array<T, N> stack;
    std::size_t capacity;
    T* heap;
};
