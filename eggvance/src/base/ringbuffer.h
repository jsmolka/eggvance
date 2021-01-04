#pragma once

#include <array>

template<typename T, std::size_t N>
class RingBufferIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using reference         = T&;
    using pointer           = T*;

    RingBufferIterator(T* begin, std::size_t index)
        : begin(begin), index(index) {}

    reference operator*()
    {
        return begin[index];
    }

    RingBufferIterator& operator++()
    {
        index = (index + 1) % N;

        return *this;
    }

    bool operator==(const RingBufferIterator& other) const
    {
        return begin == other.begin && index == other.index;
    }

    bool operator!=(const RingBufferIterator& other) const
    {
        return begin != other.begin || index != other.index;
    }

private:
    T* begin;
    std::size_t index;
};

template<typename T, std::size_t N>
class RingBuffer
{
public:
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const reference;
    using pointer         = value_type*;
    using const_pointer   = const pointer;
    using iterator        = RingBufferIterator<T, N>;
    using const_iterator  = const iterator;

    constexpr std::size_t capacity() const
    {
        return N;
    }

    std::size_t size() const
    {
        return length;
    }

    T& read()
    {
        length--;
        return data[idx_rd++ % N];
    }

    void write(const T& value)
    {
        if (length == N)
            idx_rd = (idx_rd + 1) % N;
        else
            length++;

        data[idx_wr++ % N] = value;
    }

    reference front()
    {
        return data[idx_rd];
    }

    const_reference front() const
    {
        return data[idx_rd];
    }

    reference back()
    {
        return data[(idx_rd + length) % N];
    }

    const_reference back() const
    {
        return data[(idx_rd + length) % N];
    }

    void clear()
    {
        length = 0;
        idx_rd = 0;
        idx_wr = 0;
    }

    iterator begin()
    {
        return RingBufferIterator<T, N>(data.data(), idx_rd);
    }

    iterator end()
    {
        return RingBufferIterator<T, N>(data.data(), (idx_rd + length) % N);
    }

    const_iterator begin() const
    {
        return RingBufferIterator<T, N>(data.data(), idx_rd);
    }

    const_iterator end() const
    {
        return RingBufferIterator<T, N>(data.data(), (idx_rd + length) % N);
    }

    const_iterator cbegin() const
    {
        return RingBufferIterator<T, N>(data.data(), idx_rd);
    }

    const_iterator cend() const
    {
        return RingBufferIterator<T, N>(data.data(), (idx_rd + length) % N);
    }

private:
    std::size_t length = 0;
    std::size_t idx_rd = 0;
    std::size_t idx_wr = 0;
    std::array<T, N> data{};
};
