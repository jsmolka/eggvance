#pragma once

#include <array>
#include <optional>

template<typename T, std::size_t N>
class Fifo
{
public:
    constexpr std::size_t capacity() const
    {
        return N;
    }

    std::size_t size() const
    {
        return length;
    }

    bool needsRefill() const
    {
        return length <= 16;
    }

    std::optional<T> read()
    {
        if (length == 0)
            return std::nullopt;

        length--;
        return data[idx_rd++ % N];
    }

    void write(const T& value)
    {
        if (length == N)
            idx_rd = idx_rd++ % N;
        else
            length++;

        data[idx_wr++ % N] = value;
    }

    void clear()
    {
        length = 0;
        idx_rd = 0;
        idx_wr = 0;
    }

    T sample{};

private:
    std::size_t length = 0;
    std::size_t idx_rd = 0;
    std::size_t idx_wr = 0;
    std::array<T, N> data{};
};
