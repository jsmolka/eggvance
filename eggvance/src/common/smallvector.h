#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

template<typename T, std::size_t N>
class SmallVector
{
public:
    using iterator = T*;

    SmallVector();
    ~SmallVector();

    SmallVector(const SmallVector&) = delete;
    SmallVector& operator=(const SmallVector&) = delete;

    inline T& operator[](std::size_t index);

    inline void clear();
    inline void push_back(T&& item);
    inline void push_back(const T& item);

    inline iterator begin();
    inline iterator end();

    std::size_t size;

private:
    inline void grow();

    std::array<T, N> stack;
    std::size_t capacity;
    T* heap;
};

template<typename T, std::size_t N>
inline SmallVector<T, N>::SmallVector()
    : size(0), capacity(N), heap(stack.data())
{

}

template<typename T, std::size_t N>
inline SmallVector<T, N>::~SmallVector()
{
    if (heap != stack.data())
        delete[] heap;
}

template<typename T, std::size_t N>
inline T& SmallVector<T, N>::operator[](std::size_t index)
{
    return heap[index];
}

template<typename T, std::size_t N>
inline void SmallVector<T, N>::clear()
{
    if (heap != stack.data())
    {
        delete[] heap;
        heap = stack.data();
    }
    size = 0;
    capacity = N;
}

template<typename T, std::size_t N>
inline void SmallVector<T, N>::push_back(T&& item)
{
    if (size == capacity)
        grow();

    heap[size++] = std::move(item);
}

template<typename T, std::size_t N>
inline void SmallVector<T, N>::push_back(const T& item)
{
    if (size == capacity)
        grow();

    heap[size++] = item;
}

template<typename T, std::size_t N>
inline typename SmallVector<T, N>::iterator SmallVector<T, N>::begin()
{
    return &heap[0];
}

template<typename T, std::size_t N>
inline typename SmallVector<T, N>::iterator SmallVector<T, N>::end()
{
    return &heap[size];
}

template<typename T, std::size_t N>
inline void SmallVector<T, N>::grow()
{
    capacity <<= 1;
    T* data = new T[capacity];
    std::copy(begin(), end(), data);

    if (heap != stack.data())
        delete[] heap;

    heap = data;
}
